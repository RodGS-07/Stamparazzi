#ifndef JOGADOR_H
#define JOGADOR_H

#include "Linear.h"
#include "Colisao.h"
#include "Entidade.h"
#include "Draw.h"
#include "Adesivo.h"
#include "Solido.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <set>
#include <memory>

using namespace std;

class Jogador : public Entidade{
    private:
        float cam_yaw, cam_pitch;
        bool morto;
        AABB mascara; //Sphere mascara;
        Mix_Chunk* som_morte;

    public:
        Jogador();
        Jogador(float ix, float iy, float iz, float cy, float cp);

        void setMascara(AABB m);
        AABB getMascara() const;
        float getCamYaw() const;
        float getCamPitch() const;
        bool estaVivo() const;
        void nasce_jogador(float ix, float iy, float iz);
        void morre(int& vidas);
        void desenha_mascara(int stacks = 30, int fatias = 30);
        void desenha_mira();
        XYZ centroAABB(const AABB& box);
        float distancia_ponto(const XYZ& a, const XYZ& b);
        bool RayIntersectsAABB(const XYZ& orig, const XYZ& dir, const AABB& box);
        bool detecta_adesivo(const Adesivo& a, const vector<unique_ptr<Solido>>& poligonos);
        void tirou_foto(const Adesivo& a, float dt, float& flash_alpha, bool& flash_ativo, int& vidas, const vector<unique_ptr<Solido>>& poligonos, set<int>& objetivos, set<int>& obstaculos);
        bool tenta_mover(float dx, float dy, float dz, int& vidas, const vector<unique_ptr<Solido>>& poligonos, const vector<unique_ptr<Solido>>& limites);
        void prende_camera();
        void move_camera(float dist, float dir, float dt, int& vidas, const vector<unique_ptr<Solido>>& poligonos, const vector<unique_ptr<Solido>>& limites, float val = 0.0f);
        void controle_camera(float move_vel, float camera_sens, float dt, int& vidas, bool pause, SDL_Window* window, SDL_GameController* game_controller, const Uint8* state, const vector<unique_ptr<Solido>>& poligonos, const vector<unique_ptr<Solido>>& limites);
        void finaliza_som();
};

#endif