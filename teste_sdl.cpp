#include <SDL2/SDL.h>
#include <iostream>
#include <iomanip>

float normaliza_axis(Sint16 valor) {
    // Normaliza de -32768..32767 para -1.0..1.0
    if (valor >= 0) return valor / 32767.0f;
    else            return valor / 32768.0f;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "Erro ao inicializar SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (SDL_NumJoysticks() < 1) {
        std::cout << "Nenhum controle detectado." << std::endl;
        SDL_Quit();
        return 0;
    }

    // Necessário para o seu controle
    SDL_GameControllerAddMapping(
        "030081f4790000000600000000000000,USB Network Joystick,"
        "a:b2,b:b1,x:b3,y:b0,back:b8,start:b9,guide:b12,"
        "leftshoulder:b6,rightshoulder:b7,leftstick:b10,rightstick:b11,"
        "lefttrigger:b4,righttrigger:b5,"
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
            switch (e.type) {
                case SDL_QUIT:
                    rodando = false;
                    break;

                case SDL_CONTROLLERBUTTONDOWN: {
                    SDL_GameControllerButton botao = 
                        (SDL_GameControllerButton)e.cbutton.button;

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
                    break;
                }

                case SDL_CONTROLLERAXISMOTION: {
                    float valor = normaliza_axis(e.caxis.value);

                    switch (e.caxis.axis) {
                        case SDL_CONTROLLER_AXIS_LEFTX:
                            std::cout << "Left Stick X = " << valor << std::endl;
                            break;
                        case SDL_CONTROLLER_AXIS_LEFTY:
                            std::cout << "Left Stick Y = " << valor << std::endl;
                            break;
                        case SDL_CONTROLLER_AXIS_RIGHTX:
                            std::cout << "Right Stick X = " << valor << std::endl;
                            break;
                        case SDL_CONTROLLER_AXIS_RIGHTY:
                            std::cout << "Right Stick Y = " << valor << std::endl;
                            break;
                        case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                            std::cout << "LT = " << valor << std::endl;
                            if (valor > 0.2f) std::cout << ">>> LT pressionado!\n";
                            break;
                        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                            std::cout << "RT = " << valor << std::endl;
                            if (valor > 0.2f) std::cout << ">>> RT pressionado!\n";
                            break;
                        default:
                            std::cout << "Outro eixo (" << (int)e.caxis.axis 
                                      << ") = " << e.caxis.value << std::endl;
                            break;
                    }
                    break;
                }

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
