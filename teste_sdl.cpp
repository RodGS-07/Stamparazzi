#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "Erro ao inicializar SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Abre o primeiro controle disponível
    if (SDL_NumJoysticks() < 1) {
        std::cout << "Nenhum controle detectado." << std::endl;
        SDL_Quit();
        return 0;
    }

    //Necessário para o meu controle
    SDL_GameControllerAddMapping(
    "030081f4790000000600000000000000,USB Network Joystick,"
    "a:b2,b:b1,x:b3,y:b0,back:b8,start:b9,guide:b12,"
    "leftshoulder:b6,rightshoulder:b7,leftstick:b4,rightstick:b5,"
    "lefttrigger:b10,rightrigger:b11,"
    "dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,"
    "leftx:a0,lefty:a1,rightx:a2,righty:a3,"
    );

    SDL_GameController* controller = SDL_GameControllerOpen(0);
    if (!controller) {
        std::cerr << "Erro ao abrir controle: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::cout << "Controle conectado: " 
              << SDL_GameControllerName(controller) 
              << std::endl;

    SDL_Event e;
    bool rodando = true;

    while (rodando) {
        while (SDL_PollEvent(&e)) {
            SDL_GameControllerButton botao;
            switch (e.type) {
                case SDL_QUIT:
                    rodando = false;
                    break;

                case SDL_CONTROLLERBUTTONDOWN:
                botao = (SDL_GameControllerButton)e.cbutton.button;

                std::cout << "Botão pressionado: ";

                switch (botao) {
                    case SDL_CONTROLLER_BUTTON_A: std::cout << "A\n"; break;
                    case SDL_CONTROLLER_BUTTON_B: std::cout << "B\n"; break;
                    case SDL_CONTROLLER_BUTTON_X: std::cout << "X\n"; break;
                    case SDL_CONTROLLER_BUTTON_Y: std::cout << "Y\n"; break;
                    case SDL_CONTROLLER_BUTTON_BACK: std::cout << "Back\n"; break;
                    case SDL_CONTROLLER_BUTTON_START: std::cout << "Start\n"; break;
                    case SDL_CONTROLLER_BUTTON_GUIDE: std::cout << "Guide\n"; break;
                    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: std::cout << "LB\n"; break;
                    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: std::cout << "RB\n"; break;
                    case SDL_CONTROLLER_BUTTON_LEFTSTICK: std::cout << "L3\n"; break;
                    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: std::cout << "R3\n"; break;
                    case SDL_CONTROLLER_BUTTON_DPAD_UP: std::cout << "DPad Up\n"; break;
                    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: std::cout << "DPad Down\n"; break;
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: std::cout << "DPad Left\n"; break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: std::cout << "DPad Right\n"; break;
                    default: std::cout << "Outro (" << (int)botao << ")\n"; break;
                }
            
                
                /*case SDL_CONTROLLERBUTTONDOWN:
                    std::cout << "Botao pressionado: " << (int)e.cbutton.button << std::endl;
                    break;

                case SDL_CONTROLLERBUTTONUP:
                    std::cout << "Botao solto: " << (int)e.cbutton.button << std::endl;
                    break;*/

                case SDL_CONTROLLERAXISMOTION:
                    // Só imprime se o movimento for relevante
                    if (e.caxis.value > 8000 || e.caxis.value < -8000) {
                        std::cout << "Eixo " << (int)e.caxis.axis 
                                  << " = " << e.caxis.value << std::endl;
                    }
                    break;

                case SDL_CONTROLLERDEVICEADDED:
                    std::cout << "Novo controle conectado!" << std::endl;
                    break;

                case SDL_CONTROLLERDEVICEREMOVED:
                    std::cout << "Controle desconectado!" << std::endl;
                    rodando = false;
                    break;
            }
        }
        SDL_Delay(10); // evita uso excessivo de CPU
    }

    SDL_GameControllerClose(controller);
    SDL_Quit();
    return 0;
}
