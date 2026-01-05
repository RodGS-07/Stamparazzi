#include "Adesivo.h"
#include "Colisao.h"
#include "Draw.h"
#include "Entidade.h"
#include "Jogador.h"
#include "Linear.h"
#include "Solido.h"
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
#include <map>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <typeinfo>
#include <time.h>
#include <sstream>
#include <iomanip>
#include <random>

#define XBOUNDS 100.0f
#define YBOUNDS 100.0f
#define ZBOUNDS 100.0f
#define MOVE_VEL 10.0f
#define CAMERA_SENS 1.0f
#define ANALOG_SENS 1.25f

using namespace std;

enum DIFICULDADE {FACIL = 1, MEDIO = 2, DIFICIL = 3};
enum ESTADO {MENU_PRINCIPAL, AJUDA, JOGO_PRINCIPAL, SAINDO, PAUSE, VITORIA, DERROTA};

int teste = 0;
int dif = MEDIO;
int estado_atual = MENU_PRINCIPAL;
int timer = 90 + 30 * dif;
int renascer = 1 + 2 * dif;
int lista_cd = 0;
int vidas = 2 * dif - 1;
int minutos = timer / 60;
int segundos = timer % 60;
bool mouse_in = false;
bool pause = false;
bool tela_cheia = true;
bool show_overlay = false;
bool primeira_pessoa = true;
bool modo_daltonico = false;
bool flash_ativo = false;
float flash_alpha = 0.0f;
bool rodando = true;
Uint32 inicio, fim;
float dt;
const Uint8* state;

// Textura, largura, altura
struct TextoInfo {
    GLuint tex;
    int w, h;
};
map<string, TextoInfo> textos;
vector<string> Chaves = {"Tempo", "Objetivo", "Pause", "Morto", "Vida", "Lista_Cooldown"};
GLuint texBlocoBase = 0;
//GLuint textos["Tempo"].tex = 0, textos["Objetivo"].tex = 0, textos["Pause"].tex = 0, textos["Morto"].tex = 0, textos["Vida"].tex = 0, texBlocoBase = 0;
//int textos["Tempo"].w = 0, textos["Objetivo"].w = 0, textos["Pause"].w = 0, textos["Morto"].w = 0, textos["Vida"].w = 0, textos["Tempo"].h = 0, textos["Objetivo"].h = 0, textos["Pause"].h = 0, textos["Morto"].h = 0, textos["Vida"].h = 0;

vector<int> cores_solidos;
vector<bool> cores_ativadas;
unordered_map<int, GLuint> texNumero;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_GLContext glContext;
SDL_GameController* game_controller = NULL;
TTF_Font* fonte;

//Adesivo a = Adesivo(-5.0f,5.0f,10.0f,{0,0,1});

vector<unique_ptr<Solido>> solidos;
vector<unique_ptr<Solido>> limites;
set<int> objetivos, obstaculos;
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

GLuint cria_textura_numero(int numero, TTF_Font* fonte) {
    SDL_Color preto = {0,0,0,255};
    string txt = to_string(numero);
    SDL_Surface* surf = TTF_RenderText_Blended(fonte, txt.c_str(), preto);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formatted->w, formatted->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, formatted->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(formatted);
    SDL_FreeSurface(surf);

    return tex;
}

void cria_textura_bloco_base() {
    glGenTextures(1, &texBlocoBase);
    glBindTexture(GL_TEXTURE_2D, texBlocoBase);

    const int W = 64, H = 64;
    vector<unsigned char> pixels(W * H * 4);

    // Gera textura RGBA 100% branca
    for (int i = 0; i < W*H*4; i += 4) {
        pixels[i]   = 255;  // R
        pixels[i+1] = 255;  // G
        pixels[i+2] = 255;  // B
        pixels[i+3] = 255;  // A
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, pixels.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void desenhaTexto(GLuint textura, int x, int y, int largura, int altura) {
    if (!textura) return;

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura);

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1); // Coordenadas em pixels (ajuste p/ sua janela)

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // assume-se que a projeção/modelview 2D já estão configuradas pelo chamador
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f((float)x,         (float)y);
        glTexCoord2f(1, 0); glVertex2f((float)x + largura,(float)y);
        glTexCoord2f(1, 1); glVertex2f((float)x + largura,(float)y + altura);
        glTexCoord2f(0, 1); glVertex2f((float)x,         (float)y + altura);
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
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    // resolução base = 600px de altura
    float escala = (float)h / 600.0f;

    int novo_tam = max(18, (int)(18 * escala));  

    fonte = TTF_OpenFont("arial.ttf", novo_tam); // precisa de um .ttf na mesma pasta
    //cout << (fonte==NULL) << endl;
    if (!fonte) {
        std::cerr << "Erro ao carregar fonte: " << TTF_GetError() << std::endl;
        teste = -1;
    }

    for (const auto& k : Chaves) {
        textos[k] = {0, 0, 0};
    }

    SDL_Color preto = {0, 0, 0, 255}, branco = {1, 1, 1, 255};
    ostringstream oss;
    oss << "Tempo restante - " 
    << minutos << ":" << std::setw(2) << std::setfill('0') << segundos;

    string str = oss.str();
    const char* texto = str.c_str();
    textos["Tempo"].tex = criaTexturaDoTexto(texto, fonte, preto, textos["Tempo"].w, textos["Tempo"].h);

    oss.str(""); oss.clear();
    oss << "Adesivos faltando: " << objetivos.size() << " restantes";
    str = oss.str();
    texto = str.c_str();
    textos["Objetivo"].tex = criaTexturaDoTexto(texto, fonte, preto, textos["Objetivo"].w, textos["Objetivo"].h);

    ostringstream poss;
    poss << "PAUSE";
    str = poss.str();
    texto = str.c_str();
    textos["Pause"].tex = criaTexturaDoTexto(texto, fonte, preto, textos["Pause"].w, textos["Pause"].h);

    ostringstream moss;
    moss << "Voce morreu, renascendo em " << renascer;
    str = moss.str();
    texto = str.c_str();
    textos["Morto"].tex = criaTexturaDoTexto(texto, fonte, preto, textos["Morto"].w, textos["Morto"].h);

    ostringstream voss;
    voss << "Vidas restantes: " << vidas;
    str = voss.str();
    texto = str.c_str();
    textos["Vida"].tex = criaTexturaDoTexto(texto, fonte, preto, textos["Vida"].w, textos["Vida"].h);

    ostringstream coss;
    coss << "Lista disponivel";
    str = coss.str();
    texto = str.c_str();
    textos["Lista_Cooldown"].tex = criaTexturaDoTexto(texto, fonte, preto, textos["Lista_Cooldown"].w, textos["Lista_Cooldown"].h);

    cria_textura_bloco_base();
}

void atualizaTexto(const string& texto, GLuint& texturaTexto, int& larguraTexto, int& alturaTexto){
    if (texturaTexto) {
        glDeleteTextures(1, &texturaTexto); // libera a textura antiga
        texturaTexto = 0;
    }

    SDL_Color cor = {0, 0, 0, 255}; // preto
    texturaTexto = criaTexturaDoTexto(texto.c_str(), fonte, cor, larguraTexto, alturaTexto);
}

void ajusta_tamanho_fonte() {

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    // resolução base = 600px de altura
    float escala = (float)h / 600.0f;

    int novo_tam = max(18, (int)(18 * escala));   
    // 12 é o tamanho original da sua fonte
    // em fullscreen 1080p → novo_tam ≈ 21

    // fecha fonte antiga
    if (fonte) {
        TTF_CloseFont(fonte);
        fonte = nullptr;
    }

    // abre fonte nova no tamanho proporcional
    fonte = TTF_OpenFont("arial.ttf", novo_tam);
    if (!fonte) {
        cerr << "Erro ao ajustar fonte: " << TTF_GetError() << endl;
        return;
    }

    // recriar as texturas que dependem da fonte
    SDL_Color preto = {0, 0, 0, 255};

    // Tempo restante
    ostringstream oss;
    oss << "Tempo restante - "
        << minutos << ":" << setw(2) << setfill('0') << segundos;
    atualizaTexto(oss.str(), textos["Tempo"].tex, textos["Tempo"].w, textos["Tempo"].h);

    // Vidas restantes
    oss.str(""); oss.clear();
    oss << "Vidas restantes: " << vidas;
    atualizaTexto(oss.str(),textos["Vida"].tex,textos["Vida"].w,textos["Vida"].h);

    // Adesivos faltando
    oss.str(""); oss.clear();
    if(objetivos.size() != 1) oss << "Adesivos faltando: " << objetivos.size() << " restantes";
    else oss << "Adesivos faltando: 1 restante";
    atualizaTexto(oss.str(), textos["Objetivo"].tex, textos["Objetivo"].w, textos["Objetivo"].h);

    // Pause
    atualizaTexto("PAUSE", textos["Pause"].tex, textos["Pause"].w, textos["Pause"].h);

    // Renascer
    oss.str(""); oss.clear();
    oss << "Voce morreu, renascendo em " << renascer;
    atualizaTexto(oss.str(), textos["Morto"].tex, textos["Morto"].w, textos["Morto"].h);

    // Lista Cooldown
    oss.str(""); oss.clear();
    if(lista_cd) oss << "Lista disponivel em " << lista_cd;
    else oss << "Lista disponivel";
    atualizaTexto(oss.str(), textos["Lista_Cooldown"].tex, textos["Lista_Cooldown"].w, textos["Lista_Cooldown"].h);
}

void define_objetivos(int n) {
    if(dif == FACIL){
        while(objetivos.size() < n)
            objetivos.insert((rand() % (10 - 1 + 1)) + 1);
    } else {
        while(objetivos.size() < n)
            objetivos.insert((rand() % (20 - 1 + 1)) + 1);
    }

    if(dif == DIFICIL)
        for(int i = 1; i <= 20; i++)
            if(objetivos.find(i) == objetivos.end()) obstaculos.insert(i);
}

bool colisaoComSpawn(float x, float y, float z) {
    AABB spawn = {{-1,0.5f,-1}, {1,2.5f,1}};
    return (x >= spawn.min.x && x <= spawn.max.x &&
            y >= spawn.min.y && y <= spawn.max.y &&
            z >= spawn.min.z && z <= spawn.max.z);
}

void cria_solidos(int n){
    //room
    limites.push_back(make_unique<Cubo>(0.0f,0.0f,0.0f,nullptr,200.0f));

    //chao
    limites.push_back(make_unique<Cubo>(0.0f,-1.0f,0.0f,1,100.0f,0.1f,100.0f,nullptr,2.0f));

    set<int> copia; int idx;
    int randomIndex, id; auto it=copia.begin();
    for(int i=1; i<=n; i++) copia.insert(i);

    if(dif == FACIL) {
        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cubo>(
            0.0f, 1.5f, -20.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(0.0f, 1.5f, -20.0f, id),
            2.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Piramide>(
            10.0f, 1.5f, -20.0f,
            0, 0.0f, 180.0f, 0.0f,
            make_unique<Adesivo>(10.0f, 1.5f, -20.0f, id),
            4.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Esfera>(
            20.0f, 3.0f, -20.0f,
            0, 0.0f, 90.0f, 0.0f,
            make_unique<Adesivo>(20.0f, 3.0f, -20.0f, id),
            2.0f, -1.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cilindro>(
            30.0f, 1.5f, -30.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(30.0f, 1.5f, -30.0f, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cone>(
            40.0f, 0.5f, -20.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(40.0f, 0.5f, -20.0f, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cubo>(
            -20, 1.5, 50,
            0, 0.0f, 90.0f, 0.0f,
            make_unique<Adesivo>(-20, 1.5, 50, id),
            2.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Piramide>(
            20, 1.5, 50,
            0, 0.0f, 270.0f, 0.0f,
            make_unique<Adesivo>(20, 1.5, 50, id),
            4.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Esfera>(
            20.0f, 11.5f, 60.0f,
            0, -90.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(20.0f, 11.5f, 60.0f, id),
            2.0f, -1.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cilindro>(
            -30.0f, 1.5f, 30.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(-30.0f, 1.5f, 30.0f, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cone>(
            -40.0f, 0.5f, 20.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(-40.0f, 0.5f, 20.0f, id),
            2.0f, 4.0f
        ));
        copia.erase(it);
    } else if(dif == MEDIO) {
        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cubo>(
            0.0f, 1.5f, -20.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(0.0f, 1.5f, -20.0f, id),
            2.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Piramide>(
            10.0f, 1.5f, -20.0f,
            0, 0.0f, 180.0f, 0.0f,
            make_unique<Adesivo>(10.0f, 1.5f, -20.0f, id),
            4.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Esfera>(
            20.0f, 3.0f, -20.0f,
            0, 0.0f, 90.0f, 0.0f,
            make_unique<Adesivo>(20.0f, 3.0f, -20.0f, id),
            2.0f, -1.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cilindro>(
            30.0f, 1.5f, -30.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(30.0f, 1.5f, -30.0f, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cone>(
            40.0f, 0.5f, -20.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(40.0f, 0.5f, -20.0f, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cubo>(
            -20, 1.5, 50,
            0, 0.0f, 270.0f, 0.0f,
            make_unique<Adesivo>(-20, 1.5, 50, id),
            2.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Piramide>(
            20, 1.5, 50,
            0, 0.0f, 90.0f, 0.0f,
            make_unique<Adesivo>(20, 1.5, 50, id),
            4.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Esfera>(
            20.0f, 11.5f, 60.0f,
            0, 90.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(20.0f, 11.5f, 60.0f, id),
            2.0f, -1.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cilindro>(
            -30.0f, 1.5f, 30.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(-30.0f, 1.5f, 30.0f, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        auto t1 = make_unique<Torus>(
            0, -98.5, 95,
            0, 0.0f, 180.0f, 0.0f,
            make_unique<Adesivo>(0, -98.5, 95, id),
            1.0f, 3.0f
        );
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        auto t2 = make_unique<Torus>(
            0, 1.5, -95,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(0, 1.5, -95, id),
            1.0f, 3.0f
        );
        copia.erase(it);
        
        t1->setConjugado(t2.get());
        t2->setConjugado(t1.get());

        solidos.push_back(move(t1));
        solidos.push_back(move(t2));

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cubo>(
            0.0f, -98.5f, -20.0f,
            0, 0.0f, 180.0f, 0.0f,
            make_unique<Adesivo>(0.0f, -98.5f, -20.0f, id),
            2.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Piramide>(
            10.0f, -98.5f, -20.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(10.0f, -98.5f, -20.0f, id),
            4.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Esfera>(
            20.0f, -97.0f, -20.0f,
            0, 90.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(20.0f, -97.0f, -20.0f, id),
            2.0f, -100.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cilindro>(
            30.0f, -98.5f, -30.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(30.0f, -98.5f, -30.0f, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cubo>(
            -20, -98.5, 50,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(-20, -98.5, 50, id),
            2.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Piramide>(
            20, -98.5, 50,
            0, 0.0f, 270.0f, 0.0f,
            make_unique<Adesivo>(20, -98.5, 50, id),
            4.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Esfera>(
            20.0f, -88.5f, 60.0f,
            0, -90.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(20.0f, -88.5f, 60.0f, id),
            2.0f, -100.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cilindro>(
            -30.0f, -98.5f, 30.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(-30.0f, -98.5f, 30.0f, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        solidos.push_back(make_unique<Cone>(
            -40.0f, -98.5f, 20.0f,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(-40.0f, -98.5f, 20.0f, id),
            2.0f, 4.0f
        ));
        copia.erase(it);
    } else {
        float x, y, z;
        random_device rd;  
        mt19937 gen(rd()); // Mersenne Twister
        uniform_real_distribution<float> room_dist(-97.5f, 97.5f);
        uniform_real_distribution<float> ya_dist(2.5f, 97.5f);
        uniform_real_distribution<float> yb_dist(-2.5f, -97.5f);
        uniform_real_distribution<float> ang_dist(0.0f, 360.0f);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = ya_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Cubo>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            2.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = ya_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Piramide>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            4.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = ya_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Esfera>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            2.0f, -1.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = ya_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Cilindro>(
            x, y, z,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(x, y, z, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = ya_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Cone>(
            x, y, z,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(x, y, z, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = ya_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Cubo>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            2.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = ya_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Piramide>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            4.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = ya_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Esfera>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            2.0f, -1.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = ya_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Cilindro>(
            x, y, z,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(x, y, z, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        auto t1 = make_unique<Torus>(
            0, -98.5, 95,
            0, 0.0f, 180.0f, 0.0f,
            make_unique<Adesivo>(0, -98.5, 95, id),
            1.0f, 3.0f
        );
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        auto t2 = make_unique<Torus>(
            0, 1.5, -95,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(0, 1.5, -95, id),
            1.0f, 3.0f
        );
        copia.erase(it);
        
        t1->setConjugado(t2.get());
        t2->setConjugado(t1.get());

        solidos.push_back(move(t1));
        solidos.push_back(move(t2));

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = yb_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Cubo>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            2.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = yb_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Piramide>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            4.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = yb_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Esfera>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            2.0f, -100.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = yb_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Cilindro>(
            x, y, z,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(x, y, z, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = yb_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Cubo>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            2.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = yb_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Piramide>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            4.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = yb_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Esfera>(
            x, y, z,
            0, ang_dist(gen), ang_dist(gen), ang_dist(gen),
            make_unique<Adesivo>(x, y, z, id),
            2.0f, -100.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = yb_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Cilindro>(
            x, y, z,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(x, y, z, id),
            2.0f, 4.0f
        ));
        copia.erase(it);

        randomIndex = rand() % copia.size();
        it = copia.begin();
        advance(it, randomIndex);
        id = *it - 1;
        do {
            x = room_dist(gen), y = yb_dist(gen), z = room_dist(gen);
        } while(colisaoComSpawn(x,y,z));
        solidos.push_back(make_unique<Cone>(
            x, y, z,
            0, 0.0f, 0.0f, 0.0f,
            make_unique<Adesivo>(x, y, z, id),
            2.0f, 4.0f
        ));
        copia.erase(it);
    }

    cores_solidos.resize(n);

    if(dif!=FACIL){
        for(int i = 0; i < n; i++){
            cores_solidos[i] = rand() % (12+1);
            if(!cores_ativadas[cores_solidos[i]]) {i--; continue;}
            if(solidos[i]->getSuperficie()==F::TORUS and i<n-1) {
                cores_solidos[i+1]=cores_solidos[i]; i++;
            }
        }
    } else {
        for(int i = 0; i < n; i++) {
            cores_solidos[i] = rand() % (12+1);
            if(!cores_ativadas[cores_solidos[i]]) {i--; continue;}
        }
    }

    // Cria vetor de pares (polígono, cor)
    vector<pair<unique_ptr<Solido>, int>> combinados;
    for (size_t i = 0; i < solidos.size(); ++i) {
        combinados.push_back({move(solidos[i]), cores_solidos[i]});
    }

    // Ordena pelo ID do adesivo
    sort(combinados.begin(), combinados.end(),
        [](const pair<unique_ptr<Solido>, int>& a,
            const pair<unique_ptr<Solido>, int>& b) {
            if (!a.first->getAdesivo() || !b.first->getAdesivo()) return false;
            return a.first->getAdesivo()->getTexturaID() < b.first->getAdesivo()->getTexturaID();
        });

    // Reconstrói os vetores originais
    solidos.clear();
    cores_solidos.clear();
    for (auto& par : combinados) {
        solidos.push_back(move(par.first));
        cores_solidos.push_back(par.second);
    }
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
                atualizaTexto(str,textos["Tempo"].tex,textos["Tempo"].w,textos["Tempo"].h);
            }
        }
    }
}

void atualiza_objetivos(const set<int>& objetivos, const vector<int>& coresSolidos) {

    // Deleta textura antiga
    if (textos["Objetivo"].tex) {
        glDeleteTextures(1, &textos["Objetivo"].tex);
        textos["Objetivo"].tex = 0;
    }

    // Texto sempre é atualizado, mesmo se estiver vazio
    SDL_Color corTexto = {0, 0, 0, 255};
    TTF_Font* fonteLocal = fonte;

    std::ostringstream oss;
    if(objetivos.size() != 1) oss << "Adesivos faltando: " << objetivos.size() << " restantes";
    else oss << "Adesivos faltando: 1 restante";
    std::string textoStr = oss.str();

    SDL_Surface* s = TTF_RenderText_Blended(fonteLocal, textoStr.c_str(), corTexto);
    if (!s) return;

    // Cria textura
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    textos["Objetivo"].w = s->w;
    textos["Objetivo"].h = s->h;

    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_RGBA32, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formatted->w, formatted->h,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, formatted->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    textos["Objetivo"].tex = tex;

    SDL_FreeSurface(formatted);
    SDL_FreeSurface(s);
}

void desenha_bloco(float x, float y, float tamanho, Cor corFundo, GLuint texNumero) 
{
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    glColor4f(corFundo.r, corFundo.g, corFundo.b, 1.0f);
    glBindTexture(GL_TEXTURE_2D, texBlocoBase);

    // fundo do bloco
    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2f(x, y);
        glTexCoord2f(1,0); glVertex2f(x+tamanho, y);
        glTexCoord2f(1,1); glVertex2f(x+tamanho, y+tamanho);
        glTexCoord2f(0,1); glVertex2f(x, y+tamanho);
    glEnd();

    // número por cima
    glBindTexture(GL_TEXTURE_2D, texNumero);
    glColor4f(0,0,0,1); // número sempre preto

    float margem = tamanho * 0.18f;
    float numSize = tamanho - margem*2;

    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2f(x+margem, y+margem);
        glTexCoord2f(1,0); glVertex2f(x+margem+numSize, y+margem);
        glTexCoord2f(1,1); glVertex2f(x+margem+numSize, y+margem+numSize);
        glTexCoord2f(0,1); glVertex2f(x+margem, y+margem+numSize);
    glEnd();

    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

void desenha_blocos_overlay(const set<int>& objetivos, const vector<int>& coresSolidos) {

    if (objetivos.empty()) return;

    int larguraTela, alturaTela;
    SDL_GetWindowSize(window, &larguraTela, &alturaTela);

    // ===== ESCALA GLOBAL =====
    float escala = (float)alturaTela / 600.0f;

    // Fonte proporcional
    int tamFonte = max(14, (int)(32 * escala));
    TTF_Font* fnt = TTF_OpenFont("arial.ttf", tamFonte);

    // ----- Projeção 2D -----
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, larguraTela, alturaTela, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ----- Fundo branco translúcido -----
    glColor4f(1, 1, 1, 0.55f);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(larguraTela, 0);
        glVertex2f(larguraTela, alturaTela);
        glVertex2f(0, alturaTela);
    glEnd();

    // -----------------------
    // Texto: "Adesivos faltando"
    // -----------------------
    std::ostringstream oss;
    if (objetivos.size() != 1)
        oss << "Adesivos faltando: " << objetivos.size() << " restantes";
    else
        oss << "Adesivos faltando: 1 restante";

    string texto = oss.str();
    SDL_Color preto = {0,0,0,255};

    SDL_Surface* surfTxt  = TTF_RenderText_Blended(fnt, texto.c_str(), preto);
    SDL_Surface* surfTxt32 = SDL_ConvertSurfaceFormat(surfTxt, SDL_PIXELFORMAT_RGBA32, 0);

    GLuint texTexto;
    glGenTextures(1, &texTexto);
    glBindTexture(GL_TEXTURE_2D, texTexto);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 surfTxt32->w, surfTxt32->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, surfTxt32->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_TEXTURE_2D);
    glColor4f(1,1,1,1);

    float tx = larguraTela * 0.5f - surfTxt32->w * 0.5f;
    float ty = alturaTela * (0.10f + 0.05f * escala);

    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2f(tx, ty);
        glTexCoord2f(1,0); glVertex2f(tx+surfTxt32->w, ty);
        glTexCoord2f(1,1); glVertex2f(tx+surfTxt32->w, ty+surfTxt32->h);
        glTexCoord2f(0,1); glVertex2f(tx, ty+surfTxt32->h);
    glEnd();

    SDL_FreeSurface(surfTxt);
    SDL_FreeSurface(surfTxt32);
    glDeleteTextures(1, &texTexto);

    // -----------------------
    // Blocos
    // -----------------------
    // ===== NOVA ESCALA PARA OS BLOCOS =====
    // Queremos que sempre caibam 10 blocos na horizontal.
    int maxBlocos = 10;

    // Espaçamento proporcional ao tamanho do bloco
    float espacoFator = 0.25f; // 25% do tamanho do bloco

    // Cálculo automático do maior bloco possível dentro da janela
    float blocoTam = larguraTela / (maxBlocos + (maxBlocos - 1) * espacoFator);
    float espaco   = blocoTam * espacoFator;

    // Agora alinhar ao centro
    float totalLarg = objetivos.size() * blocoTam + (objetivos.size() - 1) * espaco;
    float bx = larguraTela*0.5f - totalLarg*0.5f;

    // posição vertical proporcional à tela
    float by = alturaTela * 0.30f;
    // float blocoTam = 110 * escala;
    // float espaco   = 25  * escala;

    // float totalLarg = objetivos.size() * blocoTam + (objetivos.size()-1) * espaco;
    // float bx = larguraTela * 0.5f - totalLarg * 0.5f;
    // float by = alturaTela * (0.28f + 0.07f * escala);

    for (int objetivo : objetivos) {

        int indicePoligono = -1;

        for (size_t i = 0; i < solidos.size(); ++i) {
            auto adesivo = solidos[i]->getAdesivo();
            if (adesivo && adesivo->getTexturaID() - 2 == objetivo) {
                indicePoligono = (int)i;
                break;
            }
        }

        if (indicePoligono == -1) continue;

        Cor cor = get_cor_struct(coresSolidos[indicePoligono]);

        // ----- bloco -----
        glDisable(GL_TEXTURE_2D);
        glColor4f(cor.r, cor.g, cor.b, 1.0f);

        glBegin(GL_QUADS);
            glVertex2f(bx, by);
            glVertex2f(bx+blocoTam, by);
            glVertex2f(bx+blocoTam, by+blocoTam);
            glVertex2f(bx, by+blocoTam);
        glEnd();

        // ----- número -----
        string num = to_string(objetivo);
        SDL_Color corNum = {0,0,0,255};
        if (cor.r==0 && cor.g==0 && cor.b==0) corNum = {255,255,255,255};

        SDL_Surface* numSurf = TTF_RenderText_Blended(fnt, num.c_str(), corNum);
        SDL_Surface* num32 = SDL_ConvertSurfaceFormat(numSurf, SDL_PIXELFORMAT_RGBA32, 0);

        GLuint texNum;
        glGenTextures(1, &texNum);
        glBindTexture(GL_TEXTURE_2D, texNum);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     num32->w, num32->h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, num32->pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glEnable(GL_TEXTURE_2D);
        glColor4f(1,1,1,1);

        float nx = bx + blocoTam*0.5f - num32->w*0.5f;
        float ny = by + blocoTam*0.5f - num32->h*0.5f;

        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex2f(nx, ny);
            glTexCoord2f(1,0); glVertex2f(nx+num32->w, ny);
            glTexCoord2f(1,1); glVertex2f(nx+num32->w, ny+num32->h);
            glTexCoord2f(0,1); glVertex2f(nx, ny+num32->h);
        glEnd();

        SDL_FreeSurface(numSurf);
        SDL_FreeSurface(num32);
        glDeleteTextures(1, &texNum);

        // ----- Símbolo ColorADD -----
        if (modo_daltonico) {

            GLuint texSimbolo = coresSolidos[indicePoligono] + 23;

            float simLarg = blocoTam * 0.55f;
            float simAlt  = simLarg;

            float sx = bx + blocoTam*0.5f - simLarg*0.5f;
            float sy = by + blocoTam + (12 * escala);

            glBindTexture(GL_TEXTURE_2D, texID[texSimbolo]);
            glColor4f(1,1,1,1);

            glBegin(GL_QUADS);

            if (texSimbolo != 34) {
                glTexCoord2f(0,1); glVertex2f(sx, sy);
                glTexCoord2f(0,0); glVertex2f(sx+simLarg, sy);
                glTexCoord2f(1,0); glVertex2f(sx+simLarg, sy+simAlt);
                glTexCoord2f(1,1); glVertex2f(sx, sy+simAlt);
            } 
            else {
                glTexCoord2f(0,0); glVertex2f(sx, sy);
                glTexCoord2f(0,1); glVertex2f(sx, sy+simAlt);
                glTexCoord2f(1,1); glVertex2f(sx+simLarg, sy+simAlt);
                glTexCoord2f(1,0); glVertex2f(sx+simLarg, sy);
            }

            glEnd();
        }

        bx += blocoTam + espaco;
    }

    TTF_CloseFont(fnt);

    // ----- Restaurar -------
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
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
                renascer = 1 + 2 * dif;
                ostringstream oss;
                oss << "Voce morreu, renascendo em " << renascer;
                string str = oss.str();
                atualizaTexto(str,textos["Morto"].tex,textos["Morto"].w,textos["Morto"].h);
                if(dif != FACIL) {
                    ostringstream voss;
                    voss << "Vidas restantes: " << vidas;
                    str = voss.str();
                    atualizaTexto(str,textos["Vida"].tex,textos["Vida"].w,textos["Vida"].h);
                } else vidas = INT_MAX;
                return;
            }
            ostringstream oss;
            oss << "Voce morreu, renascendo em " << renascer;
            string str = oss.str();
            atualizaTexto(str,textos["Morto"].tex,textos["Morto"].w,textos["Morto"].h);
        }
    }
}

void atualiza_cooldown_lista(float dt) {
    static float acumulador = 0.0f;

    if(!pause) {
        acumulador += dt;
        if (acumulador >= 1.0f) { // passou 1 segundo
            acumulador -= 1.0f;
            if (lista_cd > 0) {
                lista_cd--;
                ostringstream oss;
                if(lista_cd) oss << "Lista disponivel em " << lista_cd;
                else oss << "Lista disponivel";
                string str = oss.str();
                atualizaTexto(str,textos["Lista_Cooldown"].tex,textos["Lista_Cooldown"].w,textos["Lista_Cooldown"].h);
            }
        }
    }
}

void desenha_ajuda(int ajuda_cursor, int ajuda_pagina) {
    
    enum AjudaPagina {
        AJUDA_REGRAS,
        AJUDA_CONTROLES,
        AJUDA_SOLIDOS,
        AJUDA_DIFICULDADES,
        AJUDA_MENU
    };
    
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // fundo
    glColor4f(0.5,0.5,1,1);
    glBegin(GL_QUADS);
        glVertex2i(0,0);
        glVertex2i(w,0);
        glVertex2i(w,h);
        glVertex2i(0,h);
    glEnd();

    // --- Textos ---
    SDL_Color preto = {0,0,0,255};

    int baseTitle    = 64;
    int baseOpt      = 38;
    int baseTutorial = 24;
    float escala = (float)h / 600.0f;
    
    int titleSize     = max(baseTitle, (int)round(baseTitle * escala));
    int optSize       = max(baseOpt, (int)round(baseOpt * escala));
    int tutorialSize  = max(baseTutorial, (int)round(baseTutorial * escala));

    TTF_Font* fontTitle    = TTF_OpenFont("arial.ttf", titleSize);
    TTF_Font* fontOpt      = TTF_OpenFont("arial.ttf", optSize);
    TTF_Font* fontTutorial = TTF_OpenFont("arial.ttf", tutorialSize);

    int lw, lh;
    float itemY[4] = { h * 0.30f, h * 0.40f, h * 0.50f, h * 0.60f};
    string textoItem[4] = {"REGRAS", "CONTROLES", "SOLIDOS GEOMETRICOS", "DIFICULDADES"};

    vector<vector<string>> tutorial;

    if (!game_controller) {
        tutorial = {
            {
                "No Stamparazzi, existem varios solidos geometricos",
                "que estao espalhados pela fase, e cada solido possui",
                "um adesivo numerado. O jogador deve voar pela fase e", 
                "tirar uma foto somente dos adesivos cujo numero esta",
                "na lista de objetivos. No entanto, o jogador esta",
                "submetido a um numero de vidas e um limite de tempo.",
                "Se o jogador conseguir tirar todas as fotos antes",
                "do tempo acabar, ele vence. Se o tempo acabar,",
                "ou o jogador perder todas as vidas, ele perde."
            }, 
            {
                "Use as setas ou WASD para se mover para os lados, para",
                "frente e para tras, SHIFT para voar para cima e CTRL para",
                "voar para baixo. Movimente o mouse para mudar a direcao",
                "em que voce esta olhando. Quando voce estiver perto de",
                "um adesivo, clique com o botao esquerdo do mouse para",
                "tirar uma foto. Tambem e possivel apertar ESC para abrir",
                "a lista de objetivos e P para pausar e despausar o jogo.",
                "Clicar em tela de pause faz o jogo voltar e apertar ESC",
                "enquanto o jogo esta pausado encerra a partida."
            }, 
            {
                "Existem 6 solidos geometricos no Stamparazzi:",
                "Cubos - sao paredes, estao sempre parados",
                "Piramides - estao sempre paradas, mas se o jogador",
                "tocar nelas, ele perde uma vida",
                "Esferas - estao sempre quicando; se uma delas cair",
                "em cima do jogador, ele perde uma vida",
                "Cilindros - rolam de um lado para o outro, e podem",
                "empurrar o jogador",
                "Cones - giram e disparam um laser que pode fazer",
                "o jogador perder uma vida",
                "Torus - sao portais conectados em pares; o jogador",
                "deve passar pelo aro central para ativa-los"
            }, 
            {
                "FACIL: 2 minutos para completar a fase, vidas infinitas,",
                "10 solidos, 6 objetivos e 3 segundos de penalidade ao",
                "perder uma vida",
                "MEDIO: 2 minutos e 30 segundos para completar a fase,",
                "3 vidas, 20 solidos, 8 objetivos e 5 segundos de",
                "penalidade ao perder uma vida",
                "DIFICIL: 3 minutos para completar a fase, 5 vidas, 20",
                "solidos, 10 objetivos, 7 segundos de penalidade ao",
                "perder uma vida, 10 segundos de recarga da lista de",
                "objetivos apos usa-la e cada foto errada tira 1 vida"
            }
        };
    } else {
        tutorial = {
            {
                "No Stamparazzi, existem varios solidos geometricos",
                "que estao espalhados pela fase, e cada solido possui",
                "um adesivo numerado. O jogador deve voar pela fase e", 
                "tirar uma foto somente dos adesivos cujo numero esta",
                "na lista de objetivos. No entanto, o jogador esta",
                "submetido a um numero de vidas e um limite de tempo.",
                "Se o jogador conseguir tirar todas as fotos antes",
                "do tempo acabar, ele vence. Se o tempo acabar,",
                "ou o jogador perder todas as vidas, ele perde."
            }, 
            {
                "Use os direcionais ou o analOgico esquerdo para se",
                "mover para os lados, para frente e para tras, L1/LB para",
                "voar para baixo e R1/RB para voar para cima. Movimente o",
                "analogico direito para mudar a direcao em que voce esta",
                "olhando. Quando voce estiver perto de um adesivo, aperte",
                "R2/RT para tirar uma foto. Tambem e possivel apertar",
                "SELECT para abrir a lista de objetivos e START para pausar",
                "e despausar o jogo. Apertar SELECT enquanto o jogo esta",
                "pausado encerra a partida."
            }, 
            {
                "Existem 6 solidos geometricos no Stamparazzi:",
                "Cubos - sao paredes, estao sempre parados",
                "Piramides - estao sempre paradas, mas se o jogador",
                "tocar nelas, ele perde uma vida",
                "Esferas - estao sempre quicando; se uma delas cair",
                "em cima do jogador, ele perde uma vida",
                "Cilindros - rolam de um lado para o outro, e podem",
                "empurrar o jogador",
                "Cones - giram e disparam um laser que pode fazer",
                "o jogador perder uma vida",
                "Torus - sao portais conectados em pares; o jogador",
                "deve passar pelo aro central para ativa-los"
            }, 
            {
                "FACIL: 2 minutos para completar a fase, vidas infinitas,",
                "10 solidos, 6 objetivos e 3 segundos de penalidade ao",
                "perder uma vida",
                "MEDIO: 2 minutos e 30 segundos para completar a fase,",
                "3 vidas, 20 solidos, 8 objetivos e 5 segundos de",
                "penalidade ao perder uma vida",
                "DIFICIL: 3 minutos para completar a fase, 5 vidas, 20",
                "solidos, 10 objetivos, 7 segundos de penalidade ao",
                "perder uma vida, 10 segundos de recarga da lista de",
                "objetivos apos usa-la e cada foto errada tira 1 vida"
            }
        };
    }

    if (ajuda_pagina == AJUDA_MENU) {
        // --- TÍTULO ---
        GLuint texTitulo = criaTexturaDoTexto("AJUDA", fontTitle, preto, lw, lh);
        float xTitulo = w/2 - lw/2;
        float yTitulo = h*0.10f;
        desenhaTexto(texTitulo, xTitulo, yTitulo, lw, lh);
        glDeleteTextures(1, &texTitulo);

        for (int i = 0; i < 4; i++)
        {
            GLuint tex = criaTexturaDoTexto(textoItem[i].c_str(), fontOpt, preto, lw, lh);

            float x = w/2 - lw/2;
            float y = itemY[i];

            desenhaTexto(tex, x, y, lw, lh);

            // ---------------------------------------------------
            //   DESENHAR RETÂNGULO AO REDOR DO ITEM SELECIONADO
            // ---------------------------------------------------
            if (i == ajuda_cursor)
            {
                float margem = 10.0f * escala;

                float x1 = x - margem;
                float y1 = y - margem;
                float x2 = x + lw + margem;
                float y2 = y + lh + margem;

                glColor3f(0,0,0);
                glLineWidth(3);
                glBegin(GL_LINE_LOOP);
                    glVertex2f(x1, y1);
                    glVertex2f(x2, y1);
                    glVertex2f(x2, y2);
                    glVertex2f(x1, y2);
                glEnd();
            }
            glLineWidth(1);
            glDeleteTextures(1, &tex);
        }
    } else {
        GLuint texTitulo = criaTexturaDoTexto(textoItem[ajuda_pagina].c_str(), fontTitle, preto, lw, lh);
        float xTitulo = w/2 - lw/2;
        float yTitulo = h*0.05f;
        desenhaTexto(texTitulo, xTitulo, yTitulo, lw, lh);
        glDeleteTextures(1, &texTitulo);

        GLuint texTutorial;
        float xTutorial;
        float yTutorial = h*0.20f;

        for(int i = 0; i < tutorial[ajuda_pagina].size(); i++) {
            
            texTutorial = criaTexturaDoTexto(tutorial[ajuda_pagina][i].c_str(), fontTutorial, preto, lw, lh);
            xTutorial = w/2 - lw/2; // centraliza no eixo X            
            
            desenhaTexto(texTutorial, xTutorial, yTutorial+i*30*escala, lw, lh);
        }
        glDeleteTextures(1, &texTutorial);
    }

    // restaurar cor preta para desenhar textos depois
    glColor3f(0,0,0);

    // --- TEXTO "ENTER" ---
    GLuint texEnter;
    if(ajuda_pagina == AJUDA_MENU) texEnter = !game_controller ? criaTexturaDoTexto("Pressione ESC para sair do menu de ajuda", fontOpt, preto, lw, lh) : criaTexturaDoTexto("Pressione SELECT para sair do menu de ajuda", fontOpt, preto, lw, lh);
    else texEnter = !game_controller ? criaTexturaDoTexto("Pressione qualquer tecla para voltar", fontOpt, preto, lw, lh) : criaTexturaDoTexto("Pressione qualquer botao para voltar", fontOpt, preto, lw, lh);
    desenhaTexto(texEnter, w/2 - lw/2, h*0.90f, lw, lh);
    glDeleteTextures(1, &texEnter);

    TTF_CloseFont(fontTitle);
    TTF_CloseFont(fontOpt);
    TTF_CloseFont(fontTutorial);

    // restaurar
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void loop_ajuda() {

    enum AjudaPagina {
        AJUDA_REGRAS,
        AJUDA_CONTROLES,
        AJUDA_SOLIDOS,
        AJUDA_DIFICULDADES,
        AJUDA_MENU
    };

    int ajuda_pagina = AJUDA_MENU;
    int ajuda_cursor = 0; // 0 = regras, 1 = controles, 2 = sólidos, 3 = dificuldades
    const int ajuda_opcoes = 4;

    const int DEADZONE = 16000;
    bool eixoY_ativo = false;

    SDL_Event e;

    while (estado_atual == AJUDA) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {estado_atual = MENU_PRINCIPAL; break;}

            atualiza_controller(e);

            if(!game_controller){
                if (e.type == SDL_KEYDOWN) {
                    SDL_Keycode k = e.key.keysym.sym;

                    if (ajuda_pagina == AJUDA_MENU) {
                        // mover cursor para cima / baixo
                        if (k == SDLK_UP)
                        {
                            ajuda_cursor--;
                            if (ajuda_cursor < 0) ajuda_cursor = 0;
                        }
                        else if (k == SDLK_DOWN)
                        {
                            ajuda_cursor++;
                            if (ajuda_cursor >= ajuda_opcoes) ajuda_cursor = ajuda_opcoes - 1;
                        }

                        // escolher opção
                        else if (k == SDLK_SPACE)
                        {
                            ajuda_pagina = ajuda_cursor;
                            // if (ajuda_cursor == 0)
                            // {
                            //     //estado_atual = JOGO_PRINCIPAL;
                            // }
                            // else if (ajuda_cursor == 3)
                            // {
                            //     //estado_atual = SAINDO;
                            // }
                        }
                        else if (k == SDLK_ESCAPE)
                        {
                            estado_atual = MENU_PRINCIPAL;
                        }
                    } else {
                        ajuda_pagina = AJUDA_MENU;
                    }

                    // alternar fullscreen
                    if (k == SDLK_F11)
                    {
                        tela_cheia = !tela_cheia;
                        if (tela_cheia)
                        {
                            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            int w, h; SDL_GetWindowSize(window, &w, &h);
                            ajustaProjecao(w,h);
                        }
                        else
                        {
                            SDL_SetWindowFullscreen(window, 0);
                            SDL_SetWindowSize(window, 800, 600);
                            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                            ajustaProjecao(800,600);
                        }
                        ajusta_tamanho_fonte();
                    }
                }
            } else { // game controller ativo
                if (e.type == SDL_CONTROLLERBUTTONDOWN)
                {
                    int b = e.cbutton.button;

                    if (ajuda_pagina == AJUDA_MENU) {
                        // mover cursor
                        if (b == SDL_CONTROLLER_BUTTON_DPAD_UP)
                        {
                            ajuda_cursor--;
                            if (ajuda_cursor < 0) ajuda_cursor = 0;
                        }
                        else if (b == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
                        {
                            ajuda_cursor++;
                            if (ajuda_cursor >= ajuda_opcoes) ajuda_cursor = ajuda_opcoes - 1;
                        }

                        // iniciar ou fechar jogo
                        else if (b == SDL_CONTROLLER_BUTTON_A)
                        {
                            ajuda_pagina = ajuda_cursor;
                            // if (ajuda_cursor == 0)
                            // {
                            //     estado_atual = JOGO_PRINCIPAL;
                            // }
                            // else if (ajuda_cursor == 3)
                            // {
                            //     estado_atual = SAINDO;
                            // }
                        }
                        if (b == SDL_CONTROLLER_BUTTON_BACK) {
                            estado_atual = MENU_PRINCIPAL;
                        }
                    } else {
                        ajuda_pagina = AJUDA_MENU;
                    }

                    // fullscreen
                    if (SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) &&
                        SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
                    {
                        tela_cheia = !tela_cheia;
                        if (tela_cheia)
                        {
                            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            int w,h; SDL_GetWindowSize(window,&w,&h);
                            ajustaProjecao(w,h);
                        }
                        else
                        {
                            SDL_SetWindowFullscreen(window, 0);
                            SDL_SetWindowSize(window, 800, 600);
                            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                            ajustaProjecao(800,600);
                        }
                        ajusta_tamanho_fonte();
                    }
                }
                else if (e.type == SDL_CONTROLLERAXISMOTION and ajuda_pagina == AJUDA_MENU)
                {
                    // -----------------------
                    // EIXO VERTICAL (UP/DOWN)
                    // -----------------------
                    if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
                    {
                        if (!eixoY_ativo)
                        {
                            if (e.caxis.value < -DEADZONE) // cima
                            {
                                ajuda_cursor--;
                                if (ajuda_cursor < 0) ajuda_cursor = 0;
                                eixoY_ativo = true;
                            }
                            else if (e.caxis.value > DEADZONE) // baixo
                            {
                                ajuda_cursor++;
                                if (ajuda_cursor >= ajuda_opcoes) ajuda_cursor = ajuda_opcoes - 1;
                                eixoY_ativo = true;
                            }
                        }

                        // voltou ao neutro → libera novo movimento
                        if (abs(e.caxis.value) < DEADZONE)
                            eixoY_ativo = false;
                    }
                }
            }
        }
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);

        desenha_ajuda(ajuda_cursor, ajuda_pagina);

        SDL_GL_SwapWindow(window);
    }
}

void desenha_menu(int menu_cursor, int quad_atual) {

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // fundo
    glColor4f(1,1,1,1);
    glBegin(GL_QUADS);
        glVertex2i(0,0);
        glVertex2i(w,0);
        glVertex2i(w,h);
        glVertex2i(0,h);
    glEnd();

    // --- Textos ---
    SDL_Color preto = {0,0,0,255};

    int baseTitle = 64;
    int baseOpt   = 38;
    int baseQuads = 16;
    float escala = (float)h / 600.0f;
    int titleSize = max(baseTitle, (int)round(baseTitle * escala));
    int optSize   = max(baseOpt, (int)round(baseOpt   * escala));
    int quadSize  = max(baseQuads, (int)round(baseQuads * escala));

    TTF_Font* fontTitle = TTF_OpenFont("arial.ttf", titleSize);
    TTF_Font* fontOpt   = TTF_OpenFont("arial.ttf", optSize);
    TTF_Font* fontQuads = TTF_OpenFont("arial.ttf", quadSize);

    int lw, lh;

    // --- TÍTULO ---
    GLuint texTitulo = criaTexturaDoTexto("STAMPARAZZI", fontTitle, preto, lw, lh);
    float xTitulo = w/2 - lw/2;
    float yTitulo = h*0.10f;
    desenhaTexto(texTitulo, xTitulo, yTitulo, lw, lh);
    glDeleteTextures(1, &texTitulo);

    // // --- INICIAR E FECHAR ---
    // GLuint texIniciar = criaTexturaDoTexto("INICIAR JOGO", fontOpt, preto, lw, lh);
    // float xIniciar = w/2 - lw/2;
    // float yIniciar = h*0.30f;
    // desenhaTexto(texIniciar, xIniciar, yIniciar, lw, lh);
    // glDeleteTextures(1, &texIniciar);

    // GLuint texFechar = criaTexturaDoTexto("FECHAR JOGO", fontOpt, preto, lw, lh);
    // float xFechar = w/2 - lw/2;
    // float yFechar = h*0.60f;
    // desenhaTexto(texFechar, xFechar, yFechar, lw, lh);
    // glDeleteTextures(1, &texFechar);

    // ----------------------------
    //   ITENS DO MENU (4 itens)
    // ----------------------------
    float itemY[4] = { h * 0.30f, h * 0.40f, h * 0.50f, h * 0.60f};
    string textoItem[4];

    // monta texto iniciar
    textoItem[0] = "INICIAR JOGO";

    // monta texto dificuldade
    if      (dif == FACIL)  textoItem[1] = "Dificuldade: FACIL";
    else if (dif == MEDIO)  textoItem[1] = "Dificuldade: MEDIO";
    else                    textoItem[1] = "Dificuldade: DIFICIL";

    // monta texto daltonismo
    textoItem[2] = string("Modo daltonico: ") + (modo_daltonico ? "ON" : "OFF");

    // monta texto fechar
    textoItem[3] = "FECHAR JOGO";

    for (int i = 0; i < 4; i++)
    {
        GLuint tex = criaTexturaDoTexto(textoItem[i].c_str(), fontOpt, preto, lw, lh);

        float x = w/2 - lw/2;
        float y = itemY[i];

        desenhaTexto(tex, x, y, lw, lh);

        // ---------------------------------------------------
        //   DESENHAR RETÂNGULO AO REDOR DO ITEM SELECIONADO
        // ---------------------------------------------------
        if (i == menu_cursor)
        {
            float margem = 10.0f * escala;

            float x1 = x - margem;
            float y1 = y - margem;
            float x2 = x + lw + margem;
            float y2 = y + lh + margem;

            glColor3f(0,0,0);
            glLineWidth(3);
            glBegin(GL_LINE_LOOP);
                glVertex2f(x1, y1);
                glVertex2f(x2, y1);
                glVertex2f(x2, y2);
                glVertex2f(x1, y2);
            glEnd();

            // -----------------------------
            //       SETAS LATERAIS
            // -----------------------------

            float midY = (y1 + y2) * 0.5f;   // centro vertical do item
            float arrowW = 18.0f * escala;   // largura
            float arrowH = (y2 - y1) * 0.35f; // altura proporcional ao retângulo

            // --- Seta esquerda ---
            if((menu_cursor == 1 and dif != FACIL) or (menu_cursor == 2 and modo_daltonico)) {
                glBegin(GL_TRIANGLES);
                    glVertex2f(x1 - 2*arrowW, midY);         // ponta
                    glVertex2f(x1 - arrowW, midY - arrowH);
                    glVertex2f(x1 - arrowW, midY + arrowH);
                glEnd();
            }

            // --- Seta direita ---
            if((menu_cursor == 1 and dif != DIFICIL) or (menu_cursor == 2 and !modo_daltonico)) {
                glBegin(GL_TRIANGLES);
                    glVertex2f(x2 + 2*arrowW, midY);         // ponta
                    glVertex2f(x2 + arrowW, midY - arrowH);
                    glVertex2f(x2 + arrowW, midY + arrowH);
                glEnd();
            }
        }

        glLineWidth(1);
        glDeleteTextures(1, &tex);
    }

    // --- CORES ATIVADAS ---
    GLuint texAtivadas = criaTexturaDoTexto("Escolha as cores ativadas para os solidos geometricos", fontQuads, preto, lw, lh);
    float xAtivadas = w/2 - lw/2;
    float yAtivadas = h*0.725f;
    desenhaTexto(texAtivadas, xAtivadas, yAtivadas, lw, lh);
    glDeleteTextures(1, &texAtivadas);

    // -------------------------------------------
    // DESENHAR QUADRADOS DE TODAS AS 13 CORES
    // -------------------------------------------

    int totalCores = 13;

    // tamanho base e escala
    float sqSize  = 40.0f * escala;     // tamanho do quadrado
    float sqSpace = 20.0f * escala;     // espaço entre quadrados

    float totalWidth = totalCores * sqSize + (totalCores - 1) * sqSpace;
    float startX = (w - totalWidth) * 0.5f;   // centralizar horizontalmente
    float ySquares = h * 0.775f;               // posição vertical (70% da tela)

    for (int i = 0; i < totalCores; i++)
    {
        float x1 = startX + i * (sqSize + sqSpace);
        float y1 = ySquares;
        float x2 = x1 + sqSize;
        float y2 = y1 + sqSize;

        glPushMatrix();

        // aplica a cor do polígono i
        muda_cor(i);

        glBegin(GL_QUADS);
            glVertex2f(x1, y1);
            glVertex2f(x2, y1);
            glVertex2f(x2, y2);
            glVertex2f(x1, y2);
        glEnd();

        glColor4f(0, 0, 0, 1);
        if(menu_cursor == 4 and i == quad_atual) glLineWidth(3);
        glBegin(GL_LINE_LOOP);
            glVertex2f(x1, y1);
            glVertex2f(x2, y1);
            glVertex2f(x2, y2);
            glVertex2f(x1, y2);
        glEnd();
        glLineWidth(1);

        if (modo_daltonico) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texID[i+23]);

            // centro do quad
            float cx = (x1 + x2) * 0.5f;
            float cy = (y1 + y2) * 0.5f;
            float hw = (x2 - x1) * 0.5f;
            float hh = (y2 - y1) * 0.5f;

            glPushMatrix();

            glTranslatef(cx, cy, 0);        // move origem para o centro do quad
            if(i != 11) glRotatef(90, 0, 0, 1);        // agora sim gira corretamente
            glTranslatef(-cx, -cy, 0);      // volta ao espaço original

            glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex2f(x1, y1);
                glTexCoord2f(1, 0); glVertex2f(x2, y1);
                glTexCoord2f(1, 1); glVertex2f(x2, y2);
                glTexCoord2f(0, 1); glVertex2f(x1, y2);
            glEnd();
            glPopMatrix();

            glDisable(GL_TEXTURE_2D);
        }

        // ------------------------------------------
        //   DESENHAR TEXTO "ON" / "OFF" ABAIXO DO QUAD
        // ------------------------------------------

        {
            // texto depende de cores ativadas
            const char* label = cores_ativadas[i] ? "ON" : "OFF";

            int tw, th;
            GLuint texLabel = criaTexturaDoTexto(label, fontQuads, preto, tw, th);

            // posição do texto centralizado abaixo do quadrado
            float tx = x1 + (sqSize - tw) * 0.5f;
            float ty = y2 + 5.0f * escala;  // pequeno espaçamento abaixo do quadrado

            desenhaTexto(texLabel, tx, ty, tw, th);

            glDeleteTextures(1, &texLabel);
        }

        glPopMatrix();
    }

    // restaurar cor preta para desenhar textos depois
    glColor3f(0,0,0);

    // --- TEXTO "ENTER" ---
    GLuint texEnter = !game_controller ? criaTexturaDoTexto("Pressione ESC para abrir o menu de ajuda", fontOpt, preto, lw, lh) : criaTexturaDoTexto("Pressione SELECT para abrir o menu de ajuda", fontOpt, preto, lw, lh);
    desenhaTexto(texEnter, w/2 - lw/2, h*0.90f, lw, lh);
    glDeleteTextures(1, &texEnter);

    TTF_CloseFont(fontTitle);
    TTF_CloseFont(fontOpt);
    TTF_CloseFont(fontQuads);

    // restaurar
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void loop_menu() {

    SDL_Event e;
    int menu_cursor = 0;        // 0 = iniciar, 1 = dificuldade, 2 = daltonico, 3 = fechar, 4 = cores
    const int menu_opcoes = 5;  // quantidade de itens do menu
    int quad_atual = 0;

    const int DEADZONE = 16000;
    bool eixoY_ativo = false;
    bool eixoX_ativo = false;

    while (estado_atual == MENU_PRINCIPAL) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {estado_atual = SAINDO; break;}

            atualiza_controller(e);

            if (!game_controller)
            {
                if (e.type == SDL_KEYDOWN)
                {
                    SDL_Keycode k = e.key.keysym.sym;

                    // mover cursor para cima / baixo
                    if (k == SDLK_UP)
                    {
                        menu_cursor--;
                        if (menu_cursor < 0) menu_cursor = 0;
                    }
                    else if (k == SDLK_DOWN)
                    {
                        menu_cursor++;
                        if (menu_cursor >= menu_opcoes) menu_cursor = menu_opcoes - 1;
                    }

                    // iniciar ou fechar jogo
                    else if (k == SDLK_SPACE)
                    {
                        if (menu_cursor == 0)
                        {
                            estado_atual = JOGO_PRINCIPAL;
                        }
                        else if (menu_cursor == 3)
                        {
                            estado_atual = SAINDO;
                        }
                        else if (menu_cursor == 4)
                        {
                            cores_ativadas[quad_atual] = !cores_ativadas[quad_atual];
                        }
                    }

                    // alterar opções da linha selecionada
                    else if (k == SDLK_LEFT)
                    {
                        if (menu_cursor == 1)   // dificuldade
                        {
                            dif--;
                            if (dif < FACIL) dif = FACIL;
                        }
                        else if (menu_cursor == 2) // modo daltonico
                        {
                            modo_daltonico = false;
                        }
                        else if (menu_cursor == 4)
                        {
                            quad_atual--;
                            if (quad_atual < 0) quad_atual = 0;
                        }
                    }
                    else if (k == SDLK_RIGHT)
                    {
                        if (menu_cursor == 1)
                        {
                            dif++;
                            if (dif > DIFICIL) dif = DIFICIL;
                        }
                        else if (menu_cursor == 2)
                        {
                            modo_daltonico = true;
                        }
                        else if (menu_cursor == 4)
                        {
                            quad_atual++;
                            if (quad_atual >= 13) quad_atual = 12;
                        }
                    }
                    else if (k == SDLK_ESCAPE) 
                    {
                        estado_atual = AJUDA;
                    }

                    // if (k == SDLK_SPACE and menu_cursor == 2)
                    //     cores_ativadas[quad_atual] = !cores_ativadas[quad_atual];

                    // iniciar jogo
                    // else if (k == SDLK_RETURN)
                    // {
                    //     estado_atual = JOGO_PRINCIPAL;
                    // }

                    // alternar fullscreen
                    else if (k == SDLK_F11)
                    {
                        tela_cheia = !tela_cheia;
                        if (tela_cheia)
                        {
                            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            int w, h; SDL_GetWindowSize(window, &w, &h);
                            ajustaProjecao(w,h);
                        }
                        else
                        {
                            SDL_SetWindowFullscreen(window, 0);
                            SDL_SetWindowSize(window, 800, 600);
                            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                            ajustaProjecao(800,600);
                        }
                        ajusta_tamanho_fonte();
                    }

                    // else if (k == SDLK_ESCAPE)
                    // {
                    //     estado_atual = SAINDO;
                    // }
                }
            } else // game_controller ativo
            {
                if (e.type == SDL_CONTROLLERBUTTONDOWN)
                {
                    int b = e.cbutton.button;

                    // mover cursor
                    if (b == SDL_CONTROLLER_BUTTON_DPAD_UP)
                    {
                        menu_cursor--;
                        if (menu_cursor < 0) menu_cursor = 0;
                    }
                    else if (b == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
                    {
                        menu_cursor++;
                        if (menu_cursor >= menu_opcoes) menu_cursor = menu_opcoes - 1;
                    }

                    // iniciar ou fechar jogo
                    else if (b == SDL_CONTROLLER_BUTTON_A)
                    {
                        if (menu_cursor == 0)
                        {
                            estado_atual = JOGO_PRINCIPAL;
                        }
                        else if (menu_cursor == 3)
                        {
                            estado_atual = SAINDO;
                        }
                        else if (menu_cursor == 4)
                        {
                            cores_ativadas[quad_atual] = !cores_ativadas[quad_atual];
                        }
                    }

                    // alterar opção da linha selecionada
                    else if (b == SDL_CONTROLLER_BUTTON_DPAD_LEFT ||
                            b == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
                    {
                        if (menu_cursor == 1)  // dificuldade
                        {
                            if (b == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
                            {
                                dif--;
                                if (dif < FACIL) dif = FACIL;
                            }
                            else
                            {
                                dif++;
                                if (dif > DIFICIL) dif = DIFICIL;
                            }
                        }
                        else if (menu_cursor == 2) // daltônico
                        {
                            if (b == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
                                modo_daltonico = 0;
                            else modo_daltonico = 1;
                        }
                        else if (menu_cursor == 4)
                        {
                            if (b == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
                            {
                                quad_atual--;
                                if (quad_atual < 0) quad_atual = 0;
                            }
                            else
                            {
                                quad_atual++;
                                if (quad_atual >= 13) quad_atual = 12;
                            }
                        }
                    }

                    else if (b == SDL_CONTROLLER_BUTTON_BACK)
                    {
                        estado_atual = AJUDA;
                    }

                    // fullscreen
                    if (SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) &&
                        SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
                    {
                        tela_cheia = !tela_cheia;
                        if (tela_cheia)
                        {
                            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            int w,h; SDL_GetWindowSize(window,&w,&h);
                            ajustaProjecao(w,h);
                        }
                        else
                        {
                            SDL_SetWindowFullscreen(window, 0);
                            SDL_SetWindowSize(window, 800, 600);
                            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                            ajustaProjecao(800,600);
                        }
                        ajusta_tamanho_fonte();
                    }

                    // // ativa cores desativadas
                    // if (b == SDL_CONTROLLER_BUTTON_A and menu_cursor == 4)
                    //     cores_ativadas[quad_atual] = !cores_ativadas[quad_atual]; //true;

                    // desativa cores ativadas
                    // if (b == SDL_CONTROLLER_BUTTON_B and menu_cursor == 2)
                    //     cores_ativadas[quad_atual] = false;

                    // // iniciar jogo
                    // if (b == SDL_CONTROLLER_BUTTON_START)
                    //     estado_atual = JOGO_PRINCIPAL;

                    // // sair
                    
                } else if (e.type == SDL_CONTROLLERAXISMOTION) 
                {
                    // -----------------------
                    // EIXO VERTICAL (UP/DOWN)
                    // -----------------------
                    if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
                    {
                        if (!eixoY_ativo)
                        {
                            if (e.caxis.value < -DEADZONE) // cima
                            {
                                menu_cursor--;
                                if (menu_cursor < 0) menu_cursor = 0;
                                eixoY_ativo = true;
                            }
                            else if (e.caxis.value > DEADZONE) // baixo
                            {
                                menu_cursor++;
                                if (menu_cursor >= menu_opcoes) menu_cursor = menu_opcoes - 1;
                                eixoY_ativo = true;
                            }
                        }

                        // voltou ao neutro → libera novo movimento
                        if (abs(e.caxis.value) < DEADZONE)
                            eixoY_ativo = false;
                    }

                    // -----------------------
                    // EIXO HORIZONTAL (LEFT/RIGHT)
                    // -----------------------
                    if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
                    {
                        if (!eixoX_ativo)
                        {
                            if (e.caxis.value < -DEADZONE) // esquerda
                            {
                                // mesmo código do DPAD_LEFT
                                if (menu_cursor == 1) {
                                    dif--;
                                    if (dif < FACIL) dif = FACIL;
                                }
                                else if (menu_cursor == 2) {
                                    modo_daltonico = 0;
                                }
                                else if (menu_cursor == 4) {
                                    quad_atual--;
                                    if (quad_atual < 0) quad_atual = 0;
                                }
                                eixoX_ativo = true;
                            }
                            else if (e.caxis.value > DEADZONE) // direita
                            {
                                // mesmo código do DPAD_RIGHT
                                if (menu_cursor == 1) {
                                    dif++;
                                    if (dif > DIFICIL) dif = DIFICIL;
                                }
                                else if (menu_cursor == 2) {
                                    modo_daltonico = 1;
                                }
                                else if (menu_cursor == 4) {
                                    quad_atual++;
                                    if (quad_atual >= 13) quad_atual = 12;
                                }
                                eixoX_ativo = true;
                            }
                        }

                        // voltou ao neutro
                        if (abs(e.caxis.value) < DEADZONE)
                            eixoX_ativo = false;
                    }
                }
            }
        }
        
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);

        desenha_menu(menu_cursor, quad_atual);

        SDL_GL_SwapWindow(window);
    }
}

void mostrar_resultado(string s, bool vitoria) {
    // --- Loop até o jogador pressionar ENTER ---
    SDL_Event e;

    while (true) {

        // --- Captura eventos ---
        while (SDL_PollEvent(&e)) {

            if (e.type == SDL_QUIT) {
                estado_atual = SAINDO;
                return;
            }

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    // VOLTAR PARA O MENU
                    estado_atual = MENU_PRINCIPAL;
                    return;
                }
            }
        }

        // ---------- Desenho na tela ----------

        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        glViewport(0, 0, w, h);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // ---------- FUNDO ----------
        if (vitoria)
            glColor4f(0.7f, 1.0f, 0.7f, 1.0f);  // verde claro
        else
            glColor4f(1.0f, 0.6f, 0.6f, 1.0f);  // vermelho claro

        glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(w, 0);
            glVertex2f(w, h);
            glVertex2f(0, h);
        glEnd();

        // ---------- TEXTOS ----------
        SDL_Color preto = {0,0,0,255};

        int lw, lh;

        // Fonte maior
        TTF_Font* fntBig = TTF_OpenFont("arial.ttf", 48);
        // Fonte menor
        TTF_Font* fntSmall = TTF_OpenFont("arial.ttf", 28);

        // ------- TEXTO PRINCIPAL --------
        GLuint texMsg = criaTexturaDoTexto(s.c_str(), fntBig, preto, lw, lh);
        float textX = w/2 - lw/2;
        float textY = h*0.40f;

        desenhaTexto(texMsg, textX, textY, lw, lh);
        glDeleteTextures(1, &texMsg);

        // ------- TEXTO INFERIOR -------
        string aviso = "Pressione ENTER para voltar ao menu";
        GLuint texSub = criaTexturaDoTexto(aviso.c_str(), fntSmall, preto, lw, lh);

        desenhaTexto(texSub, w/2 - lw/2, h*0.60f, lw, lh);
        glDeleteTextures(1, &texSub);

        TTF_CloseFont(fntBig);
        TTF_CloseFont(fntSmall);

        // ---------- restauração ----------
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);

        SDL_GL_SwapWindow(window);
    }
}

void loop_jogo(){

    rodando = true; pause = false;
    timer = 90 + 30 * dif;
    renascer = 1 + 2 * dif;
    if(dif == DIFICIL) lista_cd = 0;
    if(dif == FACIL) vidas = INT_MAX;
    else vidas = 2 * dif - 1;
    ostringstream oss;
    oss << "Voce morreu, renascendo em " << renascer;
    string str = oss.str();
    atualizaTexto(str,textos["Morto"].tex,textos["Morto"].w,textos["Morto"].h);
    ostringstream voss;
    voss << "Vidas restantes: " << vidas;
    str = voss.str();
    atualizaTexto(str,textos["Vida"].tex,textos["Vida"].w,textos["Vida"].h);
    ostringstream coss;
    coss << "Lista disponivel";
    str = coss.str();
    atualizaTexto(str,textos["Lista_Cooldown"].tex,textos["Lista_Cooldown"].w,textos["Lista_Cooldown"].h);

    SDL_Event evento;
    inicio = SDL_GetTicks();
    bool overlay_antes = show_overlay;

    while (rodando) {

        if(pause) show_overlay = false;
        else show_overlay = overlay_antes;

        fim = SDL_GetTicks();
        dt = (fim - inicio) / 1000.0f;
        inicio = fim;

        atualiza_timer(dt);
        if(!timer) {rodando = false; mostrar_resultado("Seu tempo acabou!", false); break;}

        if(!objetivos.size()) {rodando = false; mostrar_resultado("Voce venceu!", true); break;}
        atualiza_objetivos(objetivos, cores_solidos);

        if(lista_cd and dif==DIFICIL) atualiza_cooldown_lista(dt);

        if(!vidas) {rodando = false; mostrar_resultado("Voce perdeu todas as vidas!", false); break;}
        if(!jogador.estaVivo()) {show_overlay = false; atualiza_renascer(dt); pause=false;}

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
                        if (!pause) pause = true;
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
                        ajusta_tamanho_fonte();
                    } else if(evento.key.keysym.sym == SDLK_ESCAPE) {
                        // alterna overlay. Se quiser o comportamento antigo (trocar primeira_pessoa),
                        // use outra tecla — aqui ESC faz overlay conforme pedido.
                        //show_overlay = !show_overlay;
                        // opcional: pausar a simulação enquanto o overlay estiver ativo
                        // pause = show_overlay;
                        //SDL_SetRelativeMouseMode(show_overlay ? SDL_FALSE : SDL_TRUE);
                        //SDL_ShowCursor(show_overlay ? SDL_ENABLE : SDL_DISABLE);
                        if(!pause and jogador.estaVivo() and !lista_cd) {show_overlay = !show_overlay; overlay_antes = show_overlay;}
                        else if(pause) rodando = false;
                        if(!show_overlay and dif==DIFICIL and !lista_cd and jogador.estaVivo()) {
                            lista_cd = 10;
                            ostringstream oss;
                            if(lista_cd) oss << "Lista disponivel em " << lista_cd;
                            else oss << "Lista disponivel";
                            string str = oss.str();
                            atualizaTexto(str,textos["Lista_Cooldown"].tex,textos["Lista_Cooldown"].w,textos["Lista_Cooldown"].h);
                        }
                    }
                }
                if(evento.type == SDL_MOUSEBUTTONDOWN and pause){
                    pause = false;
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                    SDL_ShowCursor(SDL_DISABLE);
                } else if(evento.type == SDL_MOUSEBUTTONDOWN and !pause and !show_overlay and jogador.estaVivo()){
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
                        if (!pause) pause = true;
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
                        ajusta_tamanho_fonte();
                    } else if(evento.cbutton.button == SDL_CONTROLLER_BUTTON_BACK) {
                        if(!pause and jogador.estaVivo() and !lista_cd) {show_overlay = !show_overlay; overlay_antes = show_overlay;}
                        else if(pause) rodando = false;
                        if(!show_overlay and dif==DIFICIL and !lista_cd and jogador.estaVivo()) {
                            lista_cd = 10;
                            ostringstream oss;
                            if(lista_cd) oss << "Lista disponivel em " << lista_cd;
                            else oss << "Lista disponivel";
                            string str = oss.str();
                            atualizaTexto(str,textos["Lista_Cooldown"].tex,textos["Lista_Cooldown"].w,textos["Lista_Cooldown"].h);
                        }
                    }
                }
                if(SDL_GameControllerGetAxis(game_controller,SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 16000 and !pause and !show_overlay and jogador.estaVivo()){
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
        if(textos["Tempo"].tex && textos["Objetivo"].tex && textos["Pause"].tex && textos["Morto"].tex && textos["Vida"].tex && textos["Lista_Cooldown"].tex) {
            // sempre desenha tempo
            desenhaTexto(textos["Tempo"].tex, 50, 50, textos["Tempo"].w, textos["Tempo"].h);
            if(dif != FACIL) desenhaTexto(textos["Vida"].tex, 50, 75, textos["Vida"].w, textos["Vida"].h);
            if(dif == DIFICIL) desenhaTexto(textos["Lista_Cooldown"].tex, 50, 125, textos["Lista_Cooldown"].w, textos["Lista_Cooldown"].h);

            if (show_overlay) {
                int w, h;
                SDL_GetWindowSize(window, &w, &h);

                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
                gluOrtho2D(0, w, h, 0);

                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();

                glDisable(GL_LIGHTING);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                // Fundo branco translúcido
                glColor4f(1,1,1,0.5f);
                glBegin(GL_QUADS);
                    glVertex2i(0,0);
                    glVertex2i(w,0);
                    glVertex2i(w,h);
                    glVertex2i(0,h);
                glEnd();

                // Quadrados coloridos no centro
                desenha_blocos_overlay(objetivos, cores_solidos);

                glEnable(GL_DEPTH_TEST);
                glEnable(GL_LIGHTING);

                glPopMatrix();
                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
            } else {
                // modo normal: apenas desenha o texto de objetivos normalmente
                desenhaTexto(textos["Objetivo"].tex, 50, 100, textos["Objetivo"].w, textos["Objetivo"].h);
                if(pause) {
                    int larguraJanela, alturaJanela;
                    SDL_GetWindowSize(window, &larguraJanela, &alturaJanela);
                    int xCentro = (larguraJanela - textos["Pause"].w) / 2;
                    int yCentro = (alturaJanela - textos["Pause"].h) / 2;
                    desenhaTexto(textos["Pause"].tex, xCentro, yCentro, textos["Pause"].w, textos["Pause"].h);       
                } 
                if(!jogador.estaVivo()){
                    int larguraJanela, alturaJanela;
                    SDL_GetWindowSize(window, &larguraJanela, &alturaJanela);
                    int xCentro = (larguraJanela - textos["Morto"].w) / 2;
                    int yCentro = (alturaJanela - textos["Morto"].h) / 2;
                    desenhaTexto(textos["Morto"].tex, xCentro, yCentro, textos["Morto"].w, textos["Morto"].h);       
                    // if(!tela_cheia){
                    //     int larguraJanela, alturaJanela;
                    //     SDL_GetWindowSize(window, &larguraJanela, &alturaJanela);
                    //     int xCentro = (larguraJanela - textos["Pause"].w) / 2;
                    //     int yCentro = (alturaJanela - textos["Pause"].h) / 2;
                    //     desenhaTexto(textos["Morto"].tex, xCentro, yCentro, textos["Morto"].w, textos["Morto"].h);
                    // } else desenhaTexto(textos["Morto"].tex, 300, 300, textos["Morto"].w, textos["Morto"].h);
                }
            }
        }
        // if(textos["Tempo"].tex and textos["Objetivo"].tex and textos["Pause"].tex and textos["Morto"].tex) {
        //     desenhaTexto(textos["Tempo"].tex, 50, 50, textos["Tempo"].w, textos["Tempo"].h);
        //     desenhaTexto(textos["Objetivo"].tex, 50, 100, textos["Objetivo"].w, textos["Objetivo"].h);
        //     if(pause) {
        //         if(!tela_cheia){
        //             int larguraJanela, alturaJanela;
        //             SDL_GetWindowSize(window, &larguraJanela, &alturaJanela);
        //             int xCentro = (larguraJanela - textos["Pause"].w) / 2;
        //             int yCentro = (alturaJanela - textos["Pause"].h) / 2;
        //             desenhaTexto(textos["Pause"].tex, xCentro, yCentro, textos["Pause"].w, textos["Pause"].h);
        //         } else desenhaTexto(textos["Pause"].tex, 400, 300, textos["Pause"].w, textos["Pause"].h);
        //     }
        //     if(!jogador.estaVivo()){
        //         if(!tela_cheia){
        //             int larguraJanela, alturaJanela;
        //             SDL_GetWindowSize(window, &larguraJanela, &alturaJanela);
        //             int xCentro = (larguraJanela - textos["Pause"].w) / 2;
        //             int yCentro = (alturaJanela - textos["Pause"].h) / 2;
        //             desenhaTexto(textos["Morto"].tex, xCentro, yCentro, textos["Morto"].w, textos["Morto"].h);
        //         } else desenhaTexto(textos["Morto"].tex, 300, 300, textos["Morto"].w, textos["Morto"].h);
        //     }
        // }

        glRotatef(-jogador.getCamPitch(), 1.0, 0.0, 0.0); 
        glRotatef(-jogador.getCamYaw(), 0.0, 1.0, 0.0);
        glTranslatef(-(jogador.getX()),-(jogador.getY()),-(jogador.getZ()));
        // if(primeira_pessoa) {
        //     glRotatef(-jogador.getCamPitch(), 1.0, 0.0, 0.0); 
        //     glRotatef(-jogador.getCamYaw(), 0.0, 1.0, 0.0);
        //     glTranslatef(-(jogador.getX()),-(jogador.getY()),-(jogador.getZ()));        
        // } else {
        //     if(jogador.getZ()+25.0f < 100.0f) {
        //         gluLookAt(jogador.getX(),jogador.getY()+5.0f,jogador.getZ()+25.0f,
        //             jogador.getX(),jogador.getY(),jogador.getZ(),
        //             0.0f,1.0f,0.0f);
        //     } else {
        //         gluLookAt(jogador.getX(),jogador.getY()+5.0f,100.0f,
        //             jogador.getX(),jogador.getY(),jogador.getZ(),
        //             0.0f,1.0f,0.0f);
        //     }
        //     if(jogador.estaVivo()) {jogador.desenha_mascara(); jogador.desenha_mira();}
        // }

        GLfloat position0[] = { 0.0, 150.0f, 0.0f, 1.0f};
        glLightfv(GL_LIGHT0,GL_POSITION,position0);

        GLfloat position1[] = { 0.0, -150.0f, 0.0f, 1.0f};
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
        prevPos.reserve(solidos.size());
        for (const auto &p : solidos) {
            prevPos.push_back({ p->getX(), p->getY(), p->getZ() });
        }

        // 2) desenhamos polígonos e máscaras e realizamos movimentos
        for (const auto& p : solidos){
            p->realiza_movimento(dt,pause,modo_daltonico);
            //p->desenha_mascara();
            //p->desenha_adesivo();
        }

        if(jogador.estaVivo()) {
            // 3) para cada polígono, checamos swept collision contra jogador
            for (size_t i = 0; i < solidos.size(); ++i) {
                auto &p = solidos[i];
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

                        // if (p->getSuperficie()==F::ESFERA) {
                        //     // esfera empurrando de cima
                        //     if (mtv.y > 0.0f and newBox.min.y > jogador.getMascara().max.y - 0.01f) {
                        //         p->aplica_efeito(jogador, vidas);
                        //         break; // não empurra, já morreu
                        //     }
                        //     // esfera por baixo: segue fluxo normal (empurra)
                        // }

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
                        for (size_t j = 0; j < solidos.size(); ++j) {
                            if (j == i) continue; // ignora o polígono que empurrou
                            if (solidos[j]->colide_jogador(jogador.getMascara())) {
                                bad = true;
                                break;
                            }
                        }

                        for (size_t j = 0; j < limites.size(); ++j) {
                            //if (j == i) continue; // ignora o polígono que empurrou
                            if (limites[j]->colide_jogador(jogador.getMascara())) {
                                bad = true;
                                break;
                            }
                        }

                        if (bad) {
                            // não foi possível empurrar o jogador (bloqueado por outro obstáculo)
                            // voltamos o jogador para o lugar e revertamos o polígono ao antigo lugar
                            
                            if (p->getSuperficie()==F::ESFERA) {
                                // esfera empurrando de cima
                                //if (mtv.y > 0.0f and newBox.min.y > jogador.getMascara().max.y - 0.01f) {
                                    p->aplica_efeito(jogador, vidas);
                                    continue; // não empurra, já morreu
                                //}
                                // esfera por baixo: segue fluxo normal (empurra)
                            }
                            
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
        }

        int i = 0;
        for(const auto& p : solidos){
            p->desenha_solido(cores_solidos[i], pause, modo_daltonico); i++;
            //p->desenha_mascara();
        }

        if(modo_daltonico){
            i=0;
            for(const auto& p : solidos){
                glPushMatrix();
                glTranslatef(p->getX(), p->getY() + 3.0f + 2.0f*(p->getSuperficie()==F::TORUS), p->getZ());
                desenha_simbolo_coloradd(cores_solidos[i]+23);
                glPopMatrix();
                i++;
            }
        }

        // Controla câmera
        if(jogador.estaVivo()) jogador.controle_camera(MOVE_VEL,CAMERA_SENS,dt,vidas,pause,window,game_controller,state,solidos,limites);
        //jogador.controle_camera(MOVE_VEL, CAMERA_SENS,dt,pause,window,game_controller,state,limites);

        for(const auto& p : solidos){
            if(p->getSuperficie()==F::CONE)
                p->aplica_efeito(jogador,vidas);
        }

        // Verifica morte do jogador
        //if(!jogador.estaVivo() and !renascer) renascer = 3;

        for(const auto& p : solidos){
            Adesivo* ade = p->getAdesivo();
            if(ade!=nullptr){
                Adesivo a = *ade;
                if(jogador.detecta_adesivo(a,solidos) and !pause and !show_overlay and jogador.estaVivo()){
                    glDisable(GL_DEPTH_TEST);   // ignora profundidade
                    marcax(p->getX(),p->getY(),p->getZ(),jogador.getCamYaw(),jogador.getCamPitch());
                    glEnable(GL_DEPTH_TEST);    // reativa para os próximos frames
                }
                jogador.tirou_foto(a,dt,flash_alpha,flash_ativo,vidas,solidos,objetivos,obstaculos);
            }  
        }

        if(show_overlay and !pause and jogador.estaVivo()) desenha_blocos_overlay(objetivos, cores_solidos);
        
        //jogador.tirou_foto(a,dt,flash_alpha,flash_ativo);

        // Atualiza tela
        SDL_GL_SwapWindow(window);
    }
    //estado_atual = MENU_PRINCIPAL;
}

void finaliza_sdl(){
    if(game_controller) {
        SDL_GameControllerClose(game_controller);
        game_controller = NULL;
    }
    for(const auto& k : Chaves) {
        glDeleteTextures(1, &textos[k].tex);
    }
    glDeleteTextures(1, &texBlocoBase);
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

    cores_ativadas = vector<bool> (13, true);

    while(estado_atual != SAINDO) {

        SDL_SetRelativeMouseMode(SDL_TRUE);
        SDL_ShowCursor(SDL_DISABLE);

        loop_menu();

        if(estado_atual == SAINDO) break;
        else if(estado_atual == AJUDA) loop_ajuda();
        else if(estado_atual == JOGO_PRINCIPAL){
            define_objetivos(4 + 2 * dif);
            cria_solidos(dif == FACIL ? 10 : 20);
            loop_jogo();

            objetivos.clear();
            obstaculos.clear();
            solidos.clear();
            cores_solidos.clear();
            jogador.nasce_jogador(0.0f,1.5f,0.0f);
        }

        // objetivos.clear();
        // obstaculos.clear();
        // solidos.clear();
        // cores_solidos.clear();
        // jogador.nasce_jogador(0.0f,1.5f,0.0f);

        estado_atual = MENU_PRINCIPAL;
    }

    finaliza_sdl();

    return 0;
}