#include <SDL2/SDL.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK) < 0) {
        cerr << "Erro ao inicializar SDL: " << SDL_GetError() << endl;
        return -1;
    }

    cout << "Joysticks detectados: " << SDL_NumJoysticks() << endl;

    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        char guid_str[64];
        SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(i);
        SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));

        cout << "Joystick " << i << ": " << SDL_JoystickNameForIndex(i) << endl;
        cout << "GUID: " << guid_str << endl;
    }

    // 🔹 Exemplo de mapping (substitua GUID pelo do seu controle)
    // Formato: "GUID,nome,a:b0,b:b1,x:a0,y:a1,back:b6,start:b7,..."
    string mapping = 
        "030081f4790000000600000000000000,USB Network Joystick,"
        "a:b0,b:b1,x:b2,y:b3,"
        "back:b6,start:b7,"
        "guide:b8,"
        "leftshoulder:b4,rightshoulder:b5,"
        "leftstick:b9,rightstick:b10,"
        "dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,"
        "leftx:a0,lefty:a1,rightx:a2,righty:a3";

    if (SDL_GameControllerAddMapping(mapping.c_str()) == -1) {
        cerr << "Falha ao adicionar mapping: " << SDL_GetError() << endl;
    } else {
        cout << "Mapping adicionado com sucesso!" << endl;
    }

    // Agora abre como GameController
    SDL_GameController* controller = nullptr;
    if (SDL_IsGameController(0)) {
        controller = SDL_GameControllerOpen(0);
        if (controller) {
            cout << "Controle aberto como GameController!" << endl;
        } else {
            cerr << "Erro ao abrir GameController: " << SDL_GetError() << endl;
        }
    } else {
        cout << "Joystick 0 não é reconhecido como GameController." << endl;
    }

    SDL_Delay(5000); // só pra dar tempo de testar

    if (controller) SDL_GameControllerClose(controller);
    SDL_Quit();
    return 0;
}
