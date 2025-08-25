#include <SDL2/SDL.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
using namespace std;

SDL_Window* window = nullptr;
SDL_GLContext glContext;
SDL_Joystick* joystick = nullptr;
bool rodando = true;

void inicializa_sdl(){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0){
        cerr << "Erro SDL: " << SDL_GetError() << endl;
        exit(1);
    }

    window = SDL_CreateWindow("Teste Joystick",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if(!window){ cerr << "Erro janela: " << SDL_GetError() << endl; exit(1); }

    glContext = SDL_GL_CreateContext(window);

    int n = SDL_NumJoysticks();
    cout << "Joysticks detectados: " << n << endl;
    for(int i = 0; i < n; i++){
        cout << "  Nome[" << i << "]: " << SDL_JoystickNameForIndex(i) << endl;
        char guid_str[1024];
        SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(i);
        SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
        cout << "  GUID: " << guid_str << endl;
    }
}

void loop_jogo(){
    SDL_Event e;

    while(rodando){
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT) rodando = false;

            if(e.type == SDL_JOYDEVICEADDED){
                if(!joystick){
                    joystick = SDL_JoystickOpen(e.jdevice.which);
                    cout << "Joystick conectado: "
                         << SDL_JoystickName(joystick) << endl;
                }
            }

            if(e.type == SDL_JOYDEVICEREMOVED){
                if(joystick && SDL_JoystickInstanceID(joystick) == e.jdevice.which){
                    SDL_JoystickClose(joystick);
                    joystick = nullptr;
                    cout << "Joystick desconectado!" << endl;
                }
            }

            if(e.type == SDL_JOYAXISMOTION){
                cout << "Axis " << (int)e.jaxis.axis
                     << " valor: " << e.jaxis.value << endl;
            }

            if(e.type == SDL_JOYBUTTONDOWN){
                cout << "Botao " << (int)e.jbutton.button << " pressionado" << endl;
            }
        }

        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(window);
    }
}

int main(int argc, char *argv[]){

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    int num_joys = SDL_NumJoysticks();
std::cout << "Joysticks detectados: " << num_joys << std::endl;

if(num_joys > 0) {
    joystick = SDL_JoystickOpen(0);
    if (joystick) {
        std::cout << "Joystick aberto com sucesso!" << std::endl;
        std::cout << "Nome: " << SDL_JoystickName(joystick) << std::endl;
        std::cout << "Eixos: " << SDL_JoystickNumAxes(joystick) << std::endl;
        std::cout << "Botoes: " << SDL_JoystickNumButtons(joystick) << std::endl;
        std::cout << "Hats: " << SDL_JoystickNumHats(joystick) << std::endl;
        std::cout << "Trackballs: " << SDL_JoystickNumBalls(joystick) << std::endl;
    } else {
        std::cout << "Erro ao abrir joystick: " << SDL_GetError() << std::endl;
    }
}
return 0;

}
