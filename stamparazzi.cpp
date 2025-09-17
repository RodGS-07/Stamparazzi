#include "Adesivo.h"
#include "Colisao.h"
#include "Draw.h"
#include "Entidade.h"
#include "Jogador.h"
#include "Linear.h"
#include "Poligono.h"
#include <SDL2/SDL.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <typeinfo>

#define XBOUNDS 100.0f
#define YBOUNDS 100.0f
#define ZBOUNDS 100.0f
#define MOVE_VEL 10.0f
#define CAMERA_SENS 1.0f
#define ANALOG_SENS 1.25f

using namespace std;

int teste = 0;
bool mouse_in = false;
bool pause = false;
bool primeira_pessoa = true;
bool flash_ativo = false;
float flash_alpha = 0.0f;
bool rodando = true;
Uint32 inicio, fim;
float dt;
const Uint8* state;

SDL_Window* window;
SDL_GLContext glContext;
SDL_GameController* game_controller = NULL;

namespace NG{ //Namespace para Informações do Game/Jogo

    enum STATE{
        MENU_PRINCIPAL,
        JOGO_PRINCIPAL,
        PAUSE,
        VITORIA,
        DERROTA
    };
};

namespace NC{ //Namespace para Controles e Comandos
    enum C {
        FRENTE,
        TRAS,
        ESQUERDA,
        DIREITA,
        CIMA,
        BAIXO,
        CAM_ESQUERDA,
        CAM_DIREITA,
        CAM_CIMA,
        CAM_BAIXO,
        SAIR,
        PAUSAR
    }; //comandos

    void traduz_entradas(){
        
    }

    void atualiza_controller(SDL_Event evento){
        if (evento.type == SDL_CONTROLLERDEVICEADDED) {
            if (!game_controller) {
                for(int i = 0; i < SDL_NumJoysticks(); i++){
                    if(SDL_IsGameController(i)){
                        game_controller = SDL_GameControllerOpen(i);
                        break;
                    }
                }
            }
        } else if (evento.type == SDL_CONTROLLERDEVICEREMOVED) {
            if (game_controller) {
                SDL_GameControllerClose(game_controller);
                game_controller = NULL;
            }
        }
    }
};

Adesivo a = Adesivo(-5.0f,5.0f,10.0f,{0,0,1});

vector<unique_ptr<Poligono>> poligonos;
Cubo room = Cubo(0.0f,0.0f,0.0f,100.0f);

Jogador jogador = Jogador(0.0f,1.0f,0.0f,0.0f,0.0f);

void inicializa_sdl(){
    // Inicializa SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "Erro ao inicializar SDL2: " << SDL_GetError() << endl;
        teste = -1;
    }

    // Cria a janela com contexto OpenGL
    window = SDL_CreateWindow("Stamparazzi",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        cerr << "Erro ao criar janela: " << SDL_GetError() << endl;
        SDL_Quit();
        teste = -1;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        cerr << "Erro ao criar contexto OpenGL: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        teste = -1;
    }

    //Necessário para o meu controle
    SDL_GameControllerAddMapping(
        "030081f4790000000600000000000000,USB Network Joystick,"
        "a:b2,b:b1,x:b3,y:b0,back:b8,start:b9,guide:b12,"
        "leftshoulder:b6,rightshoulder:b7,leftstick:b10,rightstick:b11,"
        "lefttrigger:b4,righttrigger:b5,"
        "dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,"
        "leftx:a0,lefty:a1,rightx:a2,righty:a3,"
    );

    for(int i = 0; i < SDL_NumJoysticks(); i++){
        if(SDL_IsGameController(i)){
            game_controller = SDL_GameControllerOpen(i);
            break;
        }
    }
}

void inicializa_opengl(){
    // Configuração básica do OpenGL
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 800.0/600.0, 0.1, 250.0);
    glMatrixMode(GL_MODELVIEW);
}

void cria_poligonos(int n){
    poligonos.push_back(make_unique<Cubo>(0.0f,10.0f,-20.0f,2.0f));
    poligonos.push_back(make_unique<Piramide>(10.0f,10.0f,-20.0f,4.0f,4.0f));
    poligonos.push_back(make_unique<Esfera>(20.0f,10.0f,-20.0f,2.0f));
    poligonos.push_back(make_unique<Cilindro>(30.0f,10.0f,-20.0f,2.0f,4.0f));
    poligonos.push_back(make_unique<Cone>(40.0f,10.0f,-20.0f,2.0f,4.0f));

    auto t1 = make_unique<Torus>(-20,0,0,1.0f,3.0f);
    auto t2 = make_unique<Torus>(20,0,0,1.0f,3.0f);

    t1->setConjugado(t2.get());
    t2->setConjugado(t1.get());

    poligonos.push_back(move(t1));
    poligonos.push_back(move(t2));
    /*for(int i = 0; i < n; i++){
        poligonos.push_back();
    }*/
}

void loop_jogo(){

    SDL_Event evento;
    inicio = SDL_GetTicks();

    while (rodando) {

        fim = SDL_GetTicks();
        dt = (fim - inicio) / 1000.0f;
        inicio = fim;

        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT) {
                rodando = false;
            }

            NC::atualiza_controller(evento);
            
            if(!game_controller){
                if(evento.type == SDL_KEYDOWN){
                    if(evento.key.keysym.sym == SDLK_p){
                        pause = !pause;
                        SDL_ShowCursor(pause ? SDL_ENABLE : SDL_DISABLE);
                    } else if(evento.key.keysym.sym == SDLK_ESCAPE)
                        if(pause) rodando = false;
                        else primeira_pessoa = !primeira_pessoa;
                }
                if(evento.type == SDL_MOUSEBUTTONDOWN and pause){
                    pause = false;
                    SDL_ShowCursor(SDL_DISABLE);
                } else if(evento.type == SDL_MOUSEBUTTONDOWN and !pause){
                    if(evento.button.button == SDL_BUTTON_LEFT){
                        flash_alpha = 1.0f;
                        flash_ativo = true;
                    }
                }
            } else {
                if(evento.type == SDL_CONTROLLERBUTTONDOWN) {
                    if(evento.cbutton.button == SDL_CONTROLLER_BUTTON_START)
                        pause = !pause;
                    else if(evento.cbutton.button == SDL_CONTROLLER_BUTTON_BACK)
                        if(pause) rodando = false;
                        else primeira_pessoa = !primeira_pessoa;
                }
                if(SDL_GameControllerGetAxis(game_controller,SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 16000){
                    flash_alpha = 1.0f;
                    flash_ativo = true;
                }
            }
        }

        // -------------------- detectar trigger do controle (por frame, com rising edge) --------------------
        if (game_controller) {
            Sint16 rt = SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
            bool rtPressed = (rt > 16000); // threshold; se seu controlador usar signed axis com repouso negativo, ajuste abaixo

            static bool prevRTPressed = false; // preserva estado entre frames
            if (rtPressed && !prevRTPressed) {
                flash_alpha = 1.0f;
                flash_ativo = true;
                // não chamar jogador.tirou_foto aqui — desenhe no render loop
            }
            prevRTPressed = rtPressed;
        }

        // Limpa tela
        if(!game_controller and !pause) glClearColor(1.0f,0.0f,0.5f,1.0f);
        else if(!game_controller and pause) glClearColor(0.5f,0.0f,0.25f,1.0f);
        else if(!pause) glClearColor(0.5f,0.0f,0.5f,1.0f);
        else glClearColor(0.25f,0.0f,0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        if(primeira_pessoa) {
            glRotatef(-jogador.getCamPitch(), 1.0, 0.0, 0.0); 
            glRotatef(-jogador.getCamYaw(), 0.0, 1.0, 0.0);
            glTranslatef(-(jogador.getX()),-(jogador.getY()),-(jogador.getZ()));        
        } else {
            gluLookAt(jogador.getX(),jogador.getY()+5.0f,jogador.getZ()+25.0f,
                jogador.getX(),jogador.getY(),jogador.getZ(),
                0.0f,1.0f,0.0f);
            jogador.desenha_mascara();
        }
        jogador.desenha_mira();

        // Desenha chão
		glPushMatrix();
			glTranslatef(0,-1,0);
			glScalef(100,-0.1,100);
        	desenha_chao();
		glPopMatrix();

        for(int i = 0; i < 26; i+=2){
            muda_cor(i/2);
            glPushMatrix();
                glTranslatef(-20+i*2,5,-15);
                desenha_superficie(F::CUBO);
            glPopMatrix();
        }

        for(int i = 0; i < 6; i++){
            muda_cor(i);
            glPushMatrix();
                glTranslatef(i*10,5,-30);
                desenha_superficie(i);
            glPopMatrix();
        }

        // Desenha máscara da room
        room.desenha_mascara();

        // Desenha polígonos e máscaras
        for (const auto& p : poligonos){
            p->desenha_poligono(1);
            p->desenha_mascara();
        }

        a.desenha_adesivo();

        // Controla câmera
        jogador.controle_camera(MOVE_VEL, CAMERA_SENS,dt,pause,window,game_controller,state,poligonos);

        //AABB mascara = jogador.getMascara();
        //cout << "Depois: " << mascara.min.x << " " << mascara.min.y << " " << mascara.min.z << " " << mascara.max.x << " " << mascara.max.y << " " << mascara.max.z << endl;

        /*auto mask = jogador.getMascara();
        for (const auto& p : poligonos) {
            bool coll = p->colide_jogador(mask);
            if (coll) {
                std::cout << "COLLIDE: poly@" << p.get()
                        << " tipo=" << p->getSuperficie()
                        << " jogador_pos=("<< jogador.getX() << ","<< jogador.getY() << ","<< jogador.getZ() <<")"
                        << " mask_min=("<<mask.min.x<<","<<mask.min.y<<","<<mask.min.z<<")"
                        << " mask_max=("<<mask.max.x<<","<<mask.max.y<<","<<mask.max.z<<")\n";
                p->aplica_efeito(jogador);
            }
        }*/
        
        for (const auto& p : poligonos){
            //if(p->getSuperficie()==F::TORUS) cout << p->colide_jogador(jogador.getMascara()) << endl;
            if(p->colide_jogador(jogador.getMascara()) and p->getSuperficie()!=F::CONE){
                //cout << (p->getSuperficie()==F::TORUS) << endl;
                p->aplica_efeito(jogador);
            }
            else if(p->getSuperficie()==F::CONE)
                p->aplica_efeito(jogador);
        }

        // Verifica morte do jogador
        if(!jogador.estaVivo()) jogador.nasce_jogador(0.0f,1.0f,0.0f);

        if(jogador.detecta_adesivo(a)){
            glDisable(GL_DEPTH_TEST);   // ignora profundidade
            marcax(a.getX(),a.getY(),a.getZ());
            glEnable(GL_DEPTH_TEST);    // reativa para os próximos frames
        }

        jogador.tirou_foto(a,dt,flash_alpha,flash_ativo);

        // Atualiza tela
        SDL_GL_SwapWindow(window);
    }
}

void finaliza_sdl(){
    if(game_controller) {
        SDL_GameControllerClose(game_controller);
        game_controller = NULL;
    }
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    inicializa_sdl(); if(teste == -1) return teste;
    inicializa_opengl();

	SDL_ShowCursor(SDL_ENABLE);

    cria_poligonos(7);

    loop_jogo();

    finaliza_sdl();

    return 0;
}