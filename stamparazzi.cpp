#include "Adesivo.h"
#include "Colisao.h"
#include "Draw.h"
#include "Entidade.h"
#include "Jogador.h"
#include "Linear.h"
#include "Poligono.h"
#include "Textura.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <set>
#include <memory>
#include <algorithm>
#include <typeinfo>
#include <time.h>
#include <sstream>
#include <iomanip>

#define XBOUNDS 100.0f
#define YBOUNDS 100.0f
#define ZBOUNDS 100.0f
#define MOVE_VEL 10.0f
#define CAMERA_SENS 1.0f
#define ANALOG_SENS 1.25f

using namespace std;

int teste = 0;
int timer = 180;
int renascer = 3;
int minutos = timer / 60;
int segundos = timer % 60;
bool mouse_in = false;
bool pause = false;
bool tela_cheia = true;
bool primeira_pessoa = true;
bool flash_ativo = false;
float flash_alpha = 0.0f;
bool rodando = true;
Uint32 inicio, fim;
float dt;
const Uint8* state;
GLuint texturaTextoTempo = 0, texturaTextoObj = 0, texturaTextoPause = 0, texturaTextoMorto = 0;
int larguraTextoTempo = 0, larguraTextoObj = 0, larguraTextoPause = 0, larguraTextoMorto = 0, alturaTextoTempo = 0, alturaTextoObj = 0, alturaTextoPause = 0, alturaTextoMorto = 0;
vector<int> cores_poligonos;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_GLContext glContext;
SDL_GameController* game_controller = NULL;
TTF_Font* fonte;

namespace NG{ //Namespace para Informações do Game/Jogo

    enum STATE{
        MENU_PRINCIPAL,
        JOGO_PRINCIPAL,
        PAUSE,
        VITORIA,
        DERROTA
    };
};

//Adesivo a = Adesivo(-5.0f,5.0f,10.0f,{0,0,1});

vector<unique_ptr<Poligono>> poligonos;
vector<unique_ptr<Poligono>> limites;
set<int> objetivos;
//Cubo room (0.0f,0.0f,0.0f,nullptr,100.0f);
//Cubo chao (0.0f,0.0f,0.0f,100.0f,0.1f,100.0f,nullptr,1.0f);

Jogador jogador(0.0f,1.5f,0.0f,0.0f,0.0f);

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

GLuint criaTexturaDoTexto(const char* texto, TTF_Font* fonte, SDL_Color cor, int &largura, int &altura) {
    SDL_Surface* surface = TTF_RenderText_Blended(fonte, texto, cor);
    if (!surface) {
        std::cerr << "Erro ao renderizar texto: " << TTF_GetError() << std::endl;
        return 0;
    }

    // Converte para formato RGBA conhecido
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);

    largura = formattedSurface->w;
    altura = formattedSurface->h;

    GLuint texturaID;
    glGenTextures(1, &texturaID);
    glBindTexture(GL_TEXTURE_2D, texturaID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formattedSurface->w, formattedSurface->h,
                0, GL_RGBA, GL_UNSIGNED_BYTE, formattedSurface->pixels);

    SDL_FreeSurface(formattedSurface);
    return texturaID;
}

void desenhaTexto(GLuint textura, int x, int y, int largura, int altura) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 600, 0); // Coordenadas em pixels (ajuste p/ sua janela)

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f); // cor branca

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(x, y);
        glTexCoord2f(1, 0); glVertex2f(x + largura, y);
        glTexCoord2f(1, 1); glVertex2f(x + largura, y + altura);
        glTexCoord2f(0, 1); glVertex2f(x, y + altura);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_2D);
}

void inicializa_sdl(){
    // Inicializa SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "Erro ao inicializar SDL2: " << SDL_GetError() << endl;
        teste = -1;
    }

    // Cria a janela com contexto OpenGL
    window = SDL_CreateWindow("Stamparazzi",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (!window) {
        cerr << "Erro ao criar janela: " << SDL_GetError() << endl;
        SDL_Quit();
        teste = -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        cerr << "Erro ao criar contexto OpenGL: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        teste = -1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);  // ativa mouse relativo
    SDL_ShowCursor(SDL_DISABLE);         // esconde o cursor

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

void inicializa_opengl(int argc, char* argv[]){
    // Configuração básica do OpenGL
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;
    glViewport(0, 0, w, h);
    gluPerspective(45.0, aspect, 0.1, 300.0);
    glMatrixMode(GL_MODELVIEW);

    // // 1. Luz ambiente global mais forte
    // GLfloat globalAmbiente[] = { 0.4, 0.4, 0.4, 1.0 };
    // glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbiente);

    // // 2. Luz direcional simulando lâmpada do teto
    // GLfloat lightDir[] = { 0.0f, -1.0f, -0.3f, 0.0f };
    // GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
    // glLightfv(GL_LIGHT0, GL_POSITION, lightDir);
    // glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    // glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    // // 3. Ativa luz e material
    // glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);
    // glEnable(GL_COLOR_MATERIAL);
    // glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    // Iluminação da sala
    GLfloat globalAmbiente[] = { 0.2, 0.2, 0.2, 1.0};
    GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
    
    glLightfv(GL_LIGHT0,GL_AMBIENT,globalAmbiente);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,white);
    glLightfv(GL_LIGHT0,GL_SPECULAR,white);

    glLightfv(GL_LIGHT1,GL_AMBIENT,globalAmbiente);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,white);
    glLightfv(GL_LIGHT1,GL_SPECULAR,white);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // reflexo branco
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 96.0f);
    //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
    //glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    //glMaterialf(GL_FRONT, GL_SHININESS, 30);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CarregaTexturas();
}

void inicializa_ttf(){
    if(TTF_Init() < 0){
        cerr << "Erro ao inicializar SDL_ttf: " << TTF_GetError() << endl;
        teste = -1;
    }

    // Carregar fonte
    fonte = TTF_OpenFont("arial.ttf", 12); // precisa de um .ttf na mesma pasta
    //cout << (fonte==NULL) << endl;
    if (!fonte) {
        std::cerr << "Erro ao carregar fonte: " << TTF_GetError() << std::endl;
        teste = -1;
    }

    SDL_Color preto = {0, 0, 0, 255}, branco = {1, 1, 1, 255};
    ostringstream oss;
    oss << "Tempo restante - " 
    << minutos << ":" << std::setw(2) << std::setfill('0') << segundos;

    string str = oss.str();
    const char* texto = str.c_str();
    texturaTextoTempo = criaTexturaDoTexto(texto, fonte, preto, larguraTextoTempo, alturaTextoTempo);

    oss << "Adesivos faltando: ";
    for(int n : objetivos) {
        oss << n << " ";
    }
    str = oss.str();
    texto = str.c_str();
    texturaTextoObj = criaTexturaDoTexto(texto, fonte, preto, larguraTextoObj, alturaTextoObj);

    ostringstream poss;
    poss << "PAUSE";
    str = poss.str();
    texto = str.c_str();
    texturaTextoPause = criaTexturaDoTexto(texto, fonte, preto, larguraTextoPause, alturaTextoPause);

    ostringstream moss;
    moss << "Voce morreu, renascendo em 3";
    str = moss.str();
    texto = str.c_str();
    texturaTextoMorto = criaTexturaDoTexto(texto, fonte, preto, larguraTextoMorto, alturaTextoMorto);
}

void cria_poligonos(int n){
    //room
    limites.push_back(make_unique<Cubo>(0.0f,0.0f,0.0f,nullptr,200.0f));

    //chao
    limites.push_back(make_unique<Cubo>(0.0f,-1.0f,0.0f,100.0f,0.1f,100.0f,nullptr,2.0f));

    set<int> copia; int idx;
    for(int num : objetivos) copia.insert(num);
    while(copia.size() < 8) copia.insert((rand() % (20 - 1 + 1)) + 1);

    int randomIndex = rand() % copia.size();
    auto it = copia.begin();
    advance(it, randomIndex);
    int id = *it - 1;
    poligonos.push_back(make_unique<Cubo>(
        0.0f, 0.0f, -20.0f,
        make_unique<Adesivo>(0.0f, 0.0f, -20.0f, id, XYZ{0,0,1}),
        2.0f
    ));
    copia.erase(it);

    randomIndex = rand() % copia.size();
    it = copia.begin();
    advance(it, randomIndex);
    id = *it - 1;
    poligonos.push_back(make_unique<Piramide>(
        10.0f, 0.0f, -20.0f,
        make_unique<Adesivo>(10.0f, 0.0f, -20.0f, id, XYZ{0,0,1}),
        4.0f, 4.0f
    ));
    copia.erase(it);

    randomIndex = rand() % copia.size();
    it = copia.begin();
    advance(it, randomIndex);
    id = *it - 1;
    poligonos.push_back(make_unique<Esfera>(
        20.0f, 0.0f, -20.0f,
        make_unique<Adesivo>(20.0f, 0.0f, -20.0f, id, XYZ{0,0,1}),
        2.0f
    ));
    copia.erase(it);

    randomIndex = rand() % copia.size();
    it = copia.begin();
    advance(it, randomIndex);
    id = *it - 1;
    poligonos.push_back(make_unique<Cilindro>(
        30.0f, 0.0f, -30.0f,
        make_unique<Adesivo>(30.0f, 0.0f, -30.0f, id, XYZ{0,0,1}),
        2.0f, 4.0f
    ));
    copia.erase(it);

    randomIndex = rand() % copia.size();
    it = copia.begin();
    advance(it, randomIndex);
    id = *it - 1;
    poligonos.push_back(make_unique<Cone>(
        40.0f, 0.5f, -20.0f,
        make_unique<Adesivo>(40.0f, 5.0f, -20.0f, id, XYZ{0,0,1}),
        2.0f, 4.0f
    ));
    copia.erase(it);

    randomIndex = rand() % copia.size();
    it = copia.begin();
    advance(it, randomIndex);
    id = *it - 1;
    auto t1 = make_unique<Torus>(
        -20, -98.5, 0,
        make_unique<Adesivo>(-20, -98.5, 0, id, XYZ{0,0,1}),
        1.0f, 3.0f
    );
    copia.erase(it);

    randomIndex = rand() % copia.size();
    it = copia.begin();
    advance(it, randomIndex);
    id = *it - 1;
    auto t2 = make_unique<Torus>(
        20, 1.5, 0,
        make_unique<Adesivo>(20, 1.5, 0, id, XYZ{0,0,1}),
        1.0f, 3.0f
    );
    copia.erase(it);

    t1->setConjugado(t2.get());
    t2->setConjugado(t1.get());

    poligonos.push_back(move(t1));
    poligonos.push_back(move(t2));

    randomIndex = rand() % copia.size();
    it = copia.begin();
    advance(it, randomIndex);
    id = *it - 1;
    poligonos.push_back(make_unique<Cubo>(
        0.0f, -90.0f, -20.0f,
        2.0f, 2.0f, 2.0f,
        make_unique<Adesivo>(0.0f, -90.0f, -20.0f, id, XYZ{0,0,1}),
        2.0f
    ));
    copia.erase(it);
    
    cores_poligonos.resize(n);

    for(int i = 0; i < n; i++){
        cores_poligonos[i] = rand() % (12+1);
        if(poligonos[i]->getSuperficie()==F::TORUS) {
            cores_poligonos[i+1]=cores_poligonos[i]; i++;
        }
    }

    // Cria vetor de pares (polígono, cor)
    vector<pair<unique_ptr<Poligono>, int>> combinados;
    for (size_t i = 0; i < poligonos.size(); ++i) {
        combinados.push_back({move(poligonos[i]), cores_poligonos[i]});
    }

    // Ordena pelo ID do adesivo
    sort(combinados.begin(), combinados.end(),
        [](const pair<unique_ptr<Poligono>, int>& a,
            const pair<unique_ptr<Poligono>, int>& b) {
            if (!a.first->getAdesivo() || !b.first->getAdesivo()) return false;
            return a.first->getAdesivo()->getTexturaID() < b.first->getAdesivo()->getTexturaID();
        });

    // Reconstrói os vetores originais
    poligonos.clear();
    cores_poligonos.clear();
    for (auto& par : combinados) {
        poligonos.push_back(move(par.first));
        cores_poligonos.push_back(par.second);
    }

    /*for(int i = 0; i < n; i++){
        poligonos.push_back();
    }*/
}

void define_objetivos(int n) {
    while(objetivos.size() < n)
        objetivos.insert((rand() % (20 - 1 + 1)) + 1);
}

void ajustaProjecao(int largura, int altura) {
    if (altura == 0) altura = 1; // evita divisão por zero
    float aspect = (float)largura / (float)altura;

    glViewport(0, 0, largura, altura);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 300.0);
    glMatrixMode(GL_MODELVIEW);
}

void atualizaTexto(const string& texto, GLuint& texturaTexto, int& larguraTexto, int& alturaTexto){
    if (texturaTexto) {
        glDeleteTextures(1, &texturaTexto); // libera a textura antiga
        texturaTexto = 0;
    }

    SDL_Color cor = {0, 0, 0, 255}; // preto
    texturaTexto = criaTexturaDoTexto(texto.c_str(), fonte, cor, larguraTexto, alturaTexto);
}

void atualiza_timer(float dt){
    static float acumulador = 0.0f;

    if (!pause) {
        acumulador += dt;
        if (acumulador >= 1.0f) { // passou 1 segundo
            acumulador -= 1.0f;
            if (timer > 0) {
                timer--; minutos = timer/60; segundos = timer%60;
                ostringstream oss;
                oss << "Tempo restante - " 
                << minutos << ":" << std::setw(2) << std::setfill('0') << segundos;

                string str = oss.str();
                atualizaTexto(str,texturaTextoTempo,larguraTextoTempo,alturaTextoTempo);
            }
        }
    }
}

void atualiza_objetivos(const set<int>& objetivos, const vector<int>& coresPoligonos) {
    
    if (texturaTextoObj) {
        glDeleteTextures(1, &texturaTextoObj);
        texturaTextoObj = 0;
    }

    if (objetivos.empty()) return;

    // Define fonte e cor base
    SDL_Color corTexto = {0, 0, 0, 255}; // texto preto
    TTF_Font* fonteLocal = fonte;        // usa a fonte global

    // Monta o texto com blocos coloridos
    int larguraTotal = 0;
    int alturaMax = 0;
    vector<SDL_Surface*> partes;

    // Cria a superfície inicial com o prefixo
    SDL_Surface* prefixo = TTF_RenderText_Blended(fonteLocal, "Adesivos faltando: ", corTexto);
    partes.push_back(prefixo);
    larguraTotal += prefixo->w;
    alturaMax = prefixo->h;

    // Iterador sobre o set (para preservar ordem e evitar acesso por índice)
    for (int objetivo : objetivos) {
        int indicePoligono = -1;

        // procura qual polígono tem esse adesivo
        for (size_t i = 0; i < poligonos.size(); ++i) {
            auto adesivo = poligonos[i]->getAdesivo();
            if (adesivo && adesivo->getTexturaID()-2 == objetivo) {
                indicePoligono = (int)i;
                break;
            }
        }

        if (indicePoligono == -1) continue; // se não achou o polígono, ignora

        // Obtém a cor correspondente ao polígono
        Cor cor = get_cor(coresPoligonos[indicePoligono]);
        SDL_Color corFundo = {
            (Uint8)(cor.r * 255),
            (Uint8)(cor.g * 255),
            (Uint8)(cor.b * 255),
            255
        };

        // Cria o bloco de fundo colorido
        SDL_Surface* bloco = SDL_CreateRGBSurface(
            0, 20, prefixo->h, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
        );
        SDL_FillRect(bloco, nullptr, SDL_MapRGB(bloco->format, corFundo.r, corFundo.g, corFundo.b));

        // Número do adesivo
        string num = to_string(objetivo);
        SDL_Surface* numero;
        if (!corFundo.r && !corFundo.g && !corFundo.b)
            numero = TTF_RenderText_Blended(fonteLocal, num.c_str(), {255,255,255,255});
        else
            numero = TTF_RenderText_Blended(fonteLocal, num.c_str(), {0,0,0,255});

        // Centraliza o número sobre o bloco
        SDL_Rect dst;
        dst.x = (bloco->w - numero->w) / 2;
        dst.y = (bloco->h - numero->h) / 2;
        SDL_BlitSurface(numero, nullptr, bloco, &dst);
        SDL_FreeSurface(numero);

        partes.push_back(bloco);
        larguraTotal += bloco->w + 5;
        alturaMax = max(alturaMax, bloco->h);
    }

    // Junta tudo em uma única superfície final
    SDL_Surface* finalSurf = SDL_CreateRGBSurface(
        0, larguraTotal, alturaMax, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
    );

    int xOffset = 0;
    for (SDL_Surface* s : partes) {
        SDL_Rect dst = {xOffset, 0, s->w, s->h};
        SDL_BlitSurface(s, nullptr, finalSurf, &dst);
        xOffset += s->w + 5;
        SDL_FreeSurface(s);
    }

    // --- Converte a superfície final em textura OpenGL ---
    GLuint textura;
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);

    larguraTextoObj = finalSurf->w;
    alturaTextoObj = finalSurf->h;

    // Garante que a superfície está no formato esperado (RGBA)
    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(finalSurf, SDL_PIXELFORMAT_RGBA32, 0);

    // Cria a textura OpenGL com os pixels da superfície
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formatted->w, formatted->h, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, formatted->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    texturaTextoObj = textura;

    // Libera superfícies temporárias
    SDL_FreeSurface(formatted);
    SDL_FreeSurface(finalSurf);
}

void atualiza_renascer(float dt) {
    static float acumulador = 0.0f;

    acumulador += dt;
    if (acumulador >= 1.0f) { // passou 1 segundo
        acumulador -= 1.0f;
        if (renascer > 0) {
            renascer--;
            if (renascer == 0) {
                jogador.nasce_jogador(0.0f,1.5f,0.0f);
                renascer = 3;
                ostringstream oss;
                oss << "Voce morreu, renascendo em " << renascer;
                string str = oss.str();
                atualizaTexto(str,texturaTextoMorto,larguraTextoMorto,alturaTextoMorto);
                return;
            }
            ostringstream oss;
            oss << "Voce morreu, renascendo em " << renascer;
            string str = oss.str();
            atualizaTexto(str,texturaTextoMorto,larguraTextoMorto,alturaTextoMorto);
        }
    }
}

void loop_jogo(){

    SDL_Event evento;
    inicio = SDL_GetTicks();

    // int cores[7]; for(int i = 0; i < 7; i++){
    //     if(i < 6) cores[i] = rand() % (12+1);
    //     else cores[i] = cores[i-1];
    // }

    while (rodando) {

        fim = SDL_GetTicks();
        dt = (fim - inicio) / 1000.0f;
        inicio = fim;

        atualiza_timer(dt);
        if(!timer) {rodando = false; cout << "Seu tempo acabou!" << endl; break;}

        if(!objetivos.size()) {rodando = false; cout << "Voce venceu!" << endl; break;}
        atualiza_objetivos(objetivos, cores_poligonos);

        if(!jogador.estaVivo()) atualiza_renascer(dt);

        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT) {
                rodando = false;
            }

            atualiza_controller(evento);
            
            if(!game_controller){
                if(evento.type == SDL_KEYDOWN){
                    if(evento.key.keysym.sym == SDLK_p and jogador.estaVivo()){
                        pause = !pause;
                        SDL_SetRelativeMouseMode(pause ? SDL_FALSE : SDL_TRUE);
                        SDL_ShowCursor(pause ? SDL_ENABLE : SDL_DISABLE);
                    } else if(evento.key.keysym.sym == SDLK_F11) {
                        tela_cheia = !tela_cheia;
                        if (tela_cheia) {
                            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            int w, h;
                            SDL_GetWindowSize(window, &w, &h);
                            ajustaProjecao(w, h);
                        } else {
                            SDL_SetWindowFullscreen(window, 0);
                            SDL_SetWindowSize(window, 800, 600);
                            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                            ajustaProjecao(800, 600);
                        }
                    } else if(evento.key.keysym.sym == SDLK_ESCAPE)
                        if(pause) rodando = false;
                        else primeira_pessoa = !primeira_pessoa;
                }
                if(evento.type == SDL_MOUSEBUTTONDOWN and pause){
                    pause = false;
                    SDL_ShowCursor(SDL_DISABLE);
                } else if(evento.type == SDL_MOUSEBUTTONDOWN and !pause and jogador.estaVivo()){
                    if(evento.button.button == SDL_BUTTON_LEFT){
                        flash_alpha = 1.0f;
                        flash_ativo = true;
                    }
                }
                if (evento.type == SDL_WINDOWEVENT && evento.window.event == SDL_WINDOWEVENT_RESIZED || evento.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    ajustaProjecao(evento.window.data1, evento.window.data2);
                }
            } else {
                if(evento.type == SDL_CONTROLLERBUTTONDOWN) {
                    if(evento.cbutton.button == SDL_CONTROLLER_BUTTON_START and jogador.estaVivo())
                        pause = !pause;
                    else if(evento.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK) {
                        tela_cheia = !tela_cheia;
                        if (tela_cheia) {
                            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            int w, h;
                            SDL_GetWindowSize(window, &w, &h);
                            ajustaProjecao(w, h);
                        } else {
                            SDL_SetWindowFullscreen(window, 0);
                            SDL_SetWindowSize(window, 800, 600);
                            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                            ajustaProjecao(800, 600);
                        }
                    } else if(evento.cbutton.button == SDL_CONTROLLER_BUTTON_BACK)
                        if(pause) rodando = false;
                        else primeira_pessoa = !primeira_pessoa;
                }
                if(SDL_GameControllerGetAxis(game_controller,SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 16000 and !pause and jogador.estaVivo()){
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
        if(!pause) glClearColor(1.0f,1.0f,1.0f,1.0f);
        else glClearColor(0.5f,0.5f,0.5f,1.0f);
        // if(!game_controller and !pause) glClearColor(1.0f,0.0f,0.5f,1.0f);
        // else if(!game_controller and pause) glClearColor(0.5f,0.0f,0.25f,1.0f);
        // else if(!pause) glClearColor(0.5f,0.0f,0.5f,1.0f);
        // else glClearColor(0.25f,0.0f,0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        // desenha texto
        if(texturaTextoTempo and texturaTextoObj and texturaTextoPause and texturaTextoMorto) {
            desenhaTexto(texturaTextoTempo, 50, 50, larguraTextoTempo, alturaTextoTempo);
            desenhaTexto(texturaTextoObj, 50, 100, larguraTextoObj, alturaTextoObj);
            if(pause) {
                if(!tela_cheia){
                    int larguraJanela, alturaJanela;
                    SDL_GetWindowSize(window, &larguraJanela, &alturaJanela);
                    int xCentro = (larguraJanela - larguraTextoPause) / 2;
                    int yCentro = (alturaJanela - alturaTextoPause) / 2;
                    desenhaTexto(texturaTextoPause, xCentro, yCentro, larguraTextoPause, alturaTextoPause);
                } else desenhaTexto(texturaTextoPause, 400, 300, larguraTextoPause, alturaTextoPause);
            }
            if(!jogador.estaVivo()){
                if(!tela_cheia){
                    int larguraJanela, alturaJanela;
                    SDL_GetWindowSize(window, &larguraJanela, &alturaJanela);
                    int xCentro = (larguraJanela - larguraTextoPause) / 2;
                    int yCentro = (alturaJanela - alturaTextoPause) / 2;
                    desenhaTexto(texturaTextoMorto, xCentro, yCentro, larguraTextoMorto, alturaTextoMorto);
                } else desenhaTexto(texturaTextoMorto, 300, 300, larguraTextoMorto, alturaTextoMorto);
            }
        }

        if(primeira_pessoa) {
            glRotatef(-jogador.getCamPitch(), 1.0, 0.0, 0.0); 
            glRotatef(-jogador.getCamYaw(), 0.0, 1.0, 0.0);
            glTranslatef(-(jogador.getX()),-(jogador.getY()),-(jogador.getZ()));        
        } else {
            if(jogador.getZ()+25.0f < 100.0f) {
                gluLookAt(jogador.getX(),jogador.getY()+5.0f,jogador.getZ()+25.0f,
                    jogador.getX(),jogador.getY(),jogador.getZ(),
                    0.0f,1.0f,0.0f);
            } else {
                gluLookAt(jogador.getX(),jogador.getY()+5.0f,100.0f,
                    jogador.getX(),jogador.getY(),jogador.getZ(),
                    0.0f,1.0f,0.0f);
            }
            if(jogador.estaVivo()) {jogador.desenha_mascara(); jogador.desenha_mira();}
        }

        GLfloat position0[] = { 0.0, 100.0f, 0.0f, 1.0f};
        glLightfv(GL_LIGHT0,GL_POSITION,position0);

        GLfloat position1[] = { 0.0, -100.0f, 0.0f, 1.0f};
        glLightfv(GL_LIGHT1,GL_POSITION,position1);

        // Desenha chão
		glPushMatrix();
			glTranslatef(0,-1,0);
			glScalef(100,0.1,100);
        	desenha_chao();
		glPopMatrix();

        // for(int i = 0; i < 26; i+=2){
        //     muda_cor(i/2);
        //     glPushMatrix();
        //         glTranslatef(-20+i*2,5,-15);
        //         desenha_superficie(F::CUBO);
        //     glPopMatrix();
        // }

        // for(int i = 0; i < 6; i++){
        //     muda_cor(i);
        //     glPushMatrix();
        //         glTranslatef(i*10,5,-30);
        //         desenha_superficie(i);
        //     glPopMatrix();
        // }

        // Desenha máscara da room
        //room.desenha_mascara();
        //limites[0]->desenha_mascara();
        desenha_paredes();
        //limites[1]->desenha_mascara();
        //chao.desenha_mascara();

        // 1) guardamos posições antigas
        vector<XYZ> prevPos;
        prevPos.reserve(poligonos.size());
        for (const auto &p : poligonos) {
            prevPos.push_back({ p->getX(), p->getY(), p->getZ() });
        }

        // 2) desenhamos polígonos e máscaras e realizamos movimentos
        int i = 0;
        for (const auto& p : poligonos){
            p->realiza_movimento(cores_poligonos[i],dt,pause); i++;
            //p->desenha_mascara();
            //p->desenha_adesivo();
        }

        // 3) para cada polígono, checamos swept collision contra jogador
        for (size_t i = 0; i < poligonos.size(); ++i) {
            auto &p = poligonos[i];
            AABB newBox = p->getAABB();

            // recuperar AABB antiga: set temporariamente posição anterior,
            XYZ savedPos = { p->getX(), p->getY(), p->getZ() };
            p->setX(prevPos[i].x); p->setY(prevPos[i].y); p->setZ(prevPos[i].z);
            AABB oldBox = p->getAABB();
            // restaura
            p->setX(savedPos.x); p->setY(savedPos.y); p->setZ(savedPos.z);

            // swept box = união de old e new
            AABB swept = unionAABB(oldBox, newBox);

            // se o swept AABB colide com o jogador, houve interseção no caminho potencialmente
            if (AABBvsAABB(swept, jogador.getMascara())) {
                // se já está sobreposto ao final: resolvemos com MTV
                if (AABBvsAABB(newBox, jogador.getMascara())) {
                    // calcula mtv para separar jogador do polígono (empurrar jogador)
                    XYZ mtv = computeMTV_AABB_vs_AABB(newBox, jogador.getMascara());

                    // tenta empurrar o jogador: primeiro salva estado do jogador
                    XYZ playerPrev = { jogador.getX(), jogador.getY(), jogador.getZ() };
                    AABB playerPrevMask = jogador.getMascara();

                    // aplica deslocamento no jogador
                    jogador.setX(jogador.getX() + mtv.x);
                    jogador.setY(jogador.getY() + mtv.y);
                    jogador.setZ(jogador.getZ() + mtv.z);
                    jogador.setMascara({
                        { jogador.getX() - 1.0f, jogador.getY() - 1.0f, jogador.getZ() - 1.0f },
                        { jogador.getX() + 1.0f, jogador.getY() + 1.0f, jogador.getZ() + 1.0f }
                    });

                    // verifica se, ao empurrar o jogador, ele colide com outro polígono
                    bool bad = false;
                    for (size_t j = 0; j < poligonos.size(); ++j) {
                        if (j == i) continue; // ignora o polígono que empurrou
                        if (poligonos[j]->colide_jogador(jogador.getMascara())) {
                            bad = true;
                            break;
                        }
                    }

                    if (bad) {
                        // não foi possível empurrar o jogador (bloqueado por outro obstáculo)
                        // voltamos o jogador para o lugar e revertamos o polígono ao antigo lugar
                        jogador.setX(playerPrev.x); jogador.setY(playerPrev.y); jogador.setZ(playerPrev.z);
                        jogador.setMascara(playerPrevMask);

                        p->setX(prevPos[i].x); p->setY(prevPos[i].y); p->setZ(prevPos[i].z);

                        // opcional: inverter velocidade do polígono ou zerá-la (p->vel *= -0.5f)
                        // você precisa de um método na sua classe para manipular velocidade
                    }
                    // else: empurramos com sucesso
                }
                else {
                    // swept overlapped, mas final não -- movimento passou "perto".
                    // se quiser, pode tratar amostragens/interpolação para evitar tunneling.
                }
            }
        }

        // Controla câmera
        if(jogador.estaVivo()) jogador.controle_camera(MOVE_VEL, CAMERA_SENS,dt,pause,window,game_controller,state,poligonos,limites);
        //jogador.controle_camera(MOVE_VEL, CAMERA_SENS,dt,pause,window,game_controller,state,limites);

        for(const auto& p : poligonos){
            if(p->getSuperficie()==F::CONE)
                p->aplica_efeito(jogador);
        }

        // Verifica morte do jogador
        //if(!jogador.estaVivo() and !renascer) renascer = 3;

        for(const auto& p : poligonos){
            Adesivo* ade = p->getAdesivo();
            if(ade!=nullptr){
                Adesivo a = *ade;
                if(jogador.detecta_adesivo(a,poligonos) and jogador.estaVivo()){
                    glDisable(GL_DEPTH_TEST);   // ignora profundidade
                    marcax(p->getX(),p->getY(),p->getZ(),jogador.getCamYaw(),jogador.getCamPitch());
                    glEnable(GL_DEPTH_TEST);    // reativa para os próximos frames
                }
                jogador.tirou_foto(a,dt,flash_alpha,flash_ativo,poligonos,objetivos);
            }  
        }
        
        //jogador.tirou_foto(a,dt,flash_alpha,flash_ativo);

        // Atualiza tela
        SDL_GL_SwapWindow(window);
    }
}

void finaliza_sdl(){
    if(game_controller) {
        SDL_GameControllerClose(game_controller);
        game_controller = NULL;
    }
    glDeleteTextures(1, &texturaTextoTempo);
    TTF_CloseFont(fonte);
    TTF_Quit();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    srand(time(NULL));

    inicializa_sdl(); if(teste == -1) return teste;

    inicializa_opengl(argc, argv);

    inicializa_ttf(); if(teste == -1) return teste;

	SDL_ShowCursor(SDL_ENABLE);

    define_objetivos(rand() % (8 - 1 + 1) + 1);

    cria_poligonos(8);

    loop_jogo();

    finaliza_sdl();

    return 0;
}