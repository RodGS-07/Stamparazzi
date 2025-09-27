// Código para o Jogador
#include "Jogador.h"
#include "Linear.h"
#include "Colisao.h"
#include "Entidade.h"
#include "Draw.h"
#include "Adesivo.h"
#include "Poligono.h"
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <iostream>

#define XBOUNDS 100.0f
#define YBOUNDS 100.0f
#define ZBOUNDS 100.0f
#define ANALOG_SENS 1.25f

using namespace std;

Jogador::Jogador() : Entidade() {}
Jogador::Jogador(float ix, float iy, float iz, float cy, float cp)
            : Entidade(ix,iy,iz), cam_yaw(cy), cam_pitch(cp), morto(false),
                mascara({{this->getX()-1.0f,this->getY()-1.0f,this->getZ()-1.0f},
                        {this->getX()+1.0f,this->getY()+1.0f,this->getZ()+1.0f}}) {}

void Jogador::setMascara(AABB m) {
    this->mascara = m;
    /*this->mascara = {
        { this->getX() - 1.0f, this->getY() - 1.0f, this->getZ() - 1.0f },
        { this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() + 1.0f }
    };*/
}

AABB Jogador::getMascara() const {
    return this->mascara;
    /*return {
        { this->getX() - 1.0f, this->getY() - 1.0f, this->getZ() - 1.0f },
        { this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() + 1.0f }
    };*/
}

float Jogador::getCamYaw() const {return this->cam_yaw;}

float Jogador::getCamPitch() const {return this->cam_pitch;}

bool Jogador::estaVivo() const {return !this->morto;}

void Jogador::nasce_jogador(float ix, float iy, float iz){
    this->setX(ix), this->setY(iy), this->setZ(iz);
    this->cam_yaw = this->cam_pitch = 0.0f;
    this->setMascara({
        { this->getX() - 1.0f, this->getY() - 1.0f, this->getZ() - 1.0f },
        { this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() + 1.0f }
    });
    morto = false;
}

void Jogador::morre(){
    morto = true;
}

void Jogador::desenha_mascara(int stacks, int fatias){
    muda_cor(12);

    AABB mascara = this->getMascara();
    glBegin(GL_LINE_LOOP);
    glVertex3f(mascara.min.x,mascara.min.y,mascara.min.z);
    glVertex3f(mascara.min.x,mascara.max.y,mascara.min.z);
    glVertex3f(mascara.max.x,mascara.max.y,mascara.min.z);
    glVertex3f(mascara.max.x,mascara.min.y,mascara.min.z);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(mascara.min.x,mascara.min.y,mascara.max.z);
    glVertex3f(mascara.min.x,mascara.max.y,mascara.max.z);
    glVertex3f(mascara.max.x,mascara.max.y,mascara.max.z);
    glVertex3f(mascara.max.x,mascara.min.y,mascara.max.z);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(mascara.min.x,mascara.min.y,mascara.min.z);
    glVertex3f(mascara.min.x,mascara.min.y,mascara.max.z);
    glVertex3f(mascara.max.x,mascara.min.y,mascara.max.z);
    glVertex3f(mascara.max.x,mascara.min.y,mascara.min.z);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(mascara.min.x,mascara.max.y,mascara.min.z);
    glVertex3f(mascara.min.x,mascara.max.y,mascara.max.z);
    glVertex3f(mascara.max.x,mascara.max.y,mascara.max.z);
    glVertex3f(mascara.max.x,mascara.max.y,mascara.min.z);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(mascara.min.x,mascara.min.y,mascara.min.z);
    glVertex3f(mascara.min.x,mascara.max.y,mascara.min.z);
    glVertex3f(mascara.min.x,mascara.max.y,mascara.max.z);
    glVertex3f(mascara.min.x,mascara.min.y,mascara.max.z);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(mascara.max.x,mascara.min.y,mascara.min.z);
    glVertex3f(mascara.max.x,mascara.max.y,mascara.min.z);
    glVertex3f(mascara.max.x,mascara.max.y,mascara.max.z);
    glVertex3f(mascara.max.x,mascara.min.y,mascara.max.z);
    glEnd();
    /*for (int i = 0; i < stacks; ++i) {
        float phi1 = M_PI / 2 - i * (M_PI / stacks);
        float phi2 = M_PI / 2 - (i + 1) * (M_PI / stacks);

        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < fatias; ++j) {
            float theta1 = j * (2 * M_PI / fatias);
            float theta2 = (j + 1) * (2 * M_PI / fatias);

            // Vertex 1 (bottom-left of current quad)
            float x1 = this->mascara.r * cos(phi2) * sin(theta1) + this->getX();
            float y1 = this->mascara.r * sin(phi2) + this->getY();
            float z1 = this->mascara.r * cos(phi2) * cos(theta1) + this->getZ();
            glVertex3f(x1, y1, z1);

            // Vertex 2 (bottom-right of current quad)
            float x2 = this->mascara.r * cos(phi2) * sin(theta2) + this->getX();
            float y2 = this->mascara.r * sin(phi2) + this->getY();
            float z2 = this->mascara.r * cos(phi2) * cos(theta2) + this->getZ();
            glVertex3f(x2, y2, z2);

            // Vertex 3 (top-right of current quad)
            float x3 = this->mascara.r * cos(phi1) * sin(theta2) + this->getX();
            float y3 = this->mascara.r * sin(phi1) + this->getY();
            float z3 = this->mascara.r * cos(phi1) * cos(theta2) + this->getZ();
            glVertex3f(x3, y3, z3);

            // Vertex 4 (top-left of current quad)
            float x4 = this->mascara.r * cos(phi1) * sin(theta1) + this->getX();
            float y4 = this->mascara.r * sin(phi1) + this->getY();
            float z4 = this->mascara.r * cos(phi1) * cos(theta1) + this->getZ();
            glVertex3f(x4, y4, z4);
        }
    glEnd();
    }*/
}

void Jogador::desenha_mira(){
    float radYaw   = cam_yaw   * M_PI / 180.0f;
    float radPitch = cam_pitch * M_PI / 180.0f;
    float dirX = -sin(radYaw) * cos(radPitch);
    float dirY =  sin(radPitch);
    float dirZ = -cos(radYaw) * cos(radPitch);

    muda_cor(6);
    //glLineWidth(5.0f);
    glBegin(GL_LINES);
    glVertex3f(this->getX(),this->getY(),this->getZ());
    glVertex3f(this->getX()+dirX*1000.0f,this->getY()+dirY*1000.0f,this->getZ()+dirZ*1000.0f);
    glEnd();
    //glLineWidth(1.0f);
}

bool Jogador::detecta_adesivo(const Adesivo& a){
    float radYaw   = cam_yaw   * M_PI / 180.0f;
    float radPitch = cam_pitch * M_PI / 180.0f;
    float dirX = -sin(radYaw) * cos(radPitch);
    float dirY =  sin(radPitch);
    float dirZ = -cos(radYaw) * cos(radPitch);
    XYZ vi = {this->getX(),this->getY(),this->getZ()},
        vf = {this->getX()+dirX,this->getY()+dirY,this->getZ()+dirZ},
        va = {a.getX(),a.getY(),a.getZ()};
    float grau = Arccos((vf-vi),(va-vi)) * 180.0f / M_PI;
    bool frente = (Escalar((vi-va),a.getNormal()) > 0);
    return frente and grau <= 20.0f and distancia_entidades(*this,a) <= 30.0f;
}

void Jogador::tirou_foto(const Adesivo& a, float dt, float& flash_alpha, bool& flash_ativo){
    if (flash_ativo and detecta_adesivo(a)) {
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, 800, 600, 0, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, flash_alpha);

        glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(800, 0);
            glVertex2f(800, 600);
            glVertex2f(0, 600);
        glEnd();

        //glDisable(GL_BLEND);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_DEPTH_TEST);
    }

    if (flash_ativo) {
        flash_alpha -= 2.0f * dt; // fade rápido (ajuste velocidade)
        if (flash_alpha <= 0.0f) {
            flash_alpha = 0.0f;
            flash_ativo = false;
        }
    }
}

bool Jogador::tenta_mover(float dx, float dy, float dz, const vector<unique_ptr<Poligono>>& poligonos){
    AABB candidate = this->mascara;
    //this->mascara; //Sphere candidate = this->mascara;
    candidate.min.x += dx; candidate.max.x += dx;
    candidate.min.y += dy; candidate.max.y += dy;
    candidate.min.z += dz; candidate.max.z += dz;
    //candidate.c.x += dx;
    //candidate.c.y += dy;
    //candidate.c.z += dz;

    // testa contra os limites da sala/room
    AABB a = candidate, 
        b = {{-XBOUNDS, -YBOUNDS, -ZBOUNDS},
            {XBOUNDS, YBOUNDS, ZBOUNDS}};
    if((a.min.x <= b.min.x || a.max.x >= b.max.x) ||
        (a.min.y <= b.min.y || a.max.y >= b.max.y) ||
        (a.min.z <= b.min.z || a.max.z >= b.max.z)) return false;

    // testa contra todos os poligonos (use referências para evitar cópia)    
    for (const auto& p : poligonos)
        if (p->colide_jogador(candidate)){
            if(p->getSuperficie()!=F::CONE) p->aplica_efeito(*this);
            return false; // colisão detectada => rejeita movimento
        }
        
    // sem colisão => confirma movimento
    this->setX(this->getX()+dx); //this->getX() += dx;
    this->setY(this->getY()+dy); //this->getY() += dy;
    this->setZ(this->getZ()+dz); //this->getZ() += dz;
    //touch = candidate;
    //this->setMascara(candidate);
    this->mascara = candidate;
    //cout << "Antes: " << candidate.min.x << " " << candidate.min.y << " " << candidate.min.z << " " << candidate.max.x << " " << candidate.max.y << " " << candidate.max.z << endl;
    
    return true;
}

void Jogador::prende_camera(){
    if(cam_yaw < 0.0f) cam_yaw += 360.0f;
    if(cam_yaw > 360.0f) cam_yaw -= 360.0f;
    if(cam_pitch > 90.0f) cam_pitch = 90.0f;
    if(cam_pitch < -90.0f) cam_pitch = -90.0f;
}

void Jogador::move_camera(float dist, float dir, float dt, const vector<unique_ptr<Poligono>>& poligonos, float val){
    if(dir >= 0.0f){
        float rad = (cam_yaw + dir) * M_PI / 180.0f;
        float dx = - sin(rad) * dist * dt;
        float dz = - cos(rad) * dist * dt;
        tenta_mover(dx,0.0f,dz,poligonos);
    } else {
        float dy = dist * val * dt;
        tenta_mover(0.0f,dy,0.0f,poligonos);
    }
    
}

void Jogador::controle_camera(float move_vel, float camera_sens, float dt, bool pause, SDL_Window* window, SDL_GameController* game_controller, const Uint8* state, const vector<unique_ptr<Poligono>>& poligonos){
    if(fabs(this->getX()) >= 100.0f or fabs(this->getY()) >= 100.0f or fabs(this->getZ()) >= 100.0f){
        this->morre(); return;
    }
    if(!pause and !game_controller){
        int dx, dy;
        SDL_GetRelativeMouseState(&dx, &dy);
        cam_yaw += camera_sens * -dx * dt;
        cam_pitch += camera_sens * -dy * dt;
        prende_camera();
        state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_UP] or state[SDL_SCANCODE_W])
            if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                move_camera(move_vel,0.0f,dt,poligonos);
        if(state[SDL_SCANCODE_DOWN] or state[SDL_SCANCODE_S])
            if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                move_camera(move_vel,180.0f,dt,poligonos);
        if(state[SDL_SCANCODE_LEFT] or state[SDL_SCANCODE_A])
            move_camera(move_vel,90.0f,dt,poligonos);
        if(state[SDL_SCANCODE_RIGHT] or state[SDL_SCANCODE_D])
            move_camera(move_vel,270.0f,dt,poligonos);
        if(state[SDL_SCANCODE_LSHIFT] or state[SDL_SCANCODE_RSHIFT])
            move_camera(move_vel,-1.0f,dt,poligonos,1.0f);
        if(state[SDL_SCANCODE_LCTRL] or state[SDL_SCANCODE_RCTRL])
            move_camera(move_vel,-1.0f,dt,poligonos,-1.0f);
    } else if(!pause and game_controller) {
        Sint16 axisRX = SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_RIGHTX);
        Sint16 axisRY = SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_RIGHTY);
        if(fabs(axisRX) > 16000.0f) cam_yaw   -= (static_cast<float>(axisRX) / 32767.0f) * ANALOG_SENS;  // multiplica para sensibilidade
        if(fabs(axisRY) > 16000.0f) cam_pitch -= (static_cast<float>(axisRY) / 32767.0f) * ANALOG_SENS;
        prende_camera();
        if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_UP)
            or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTY) < -16000)
            if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                move_camera(move_vel,0.0f,dt,poligonos);
        if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN)
            or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTY) > 16000)
            if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                move_camera(move_vel,180.0f,dt,poligonos);
        if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT)
            or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTX) < -16000)
            move_camera(move_vel,90.0f,dt,poligonos);
        if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
            or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTX) > 16000)
            move_camera(move_vel,270.0f,dt,poligonos);
        if(SDL_GameControllerGetButton(game_controller,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
            move_camera(move_vel,-1.0f,dt,poligonos,1.0f);
        if(SDL_GameControllerGetButton(game_controller,SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
            move_camera(move_vel,-1.0f,dt,poligonos,-1.0f);
    }
    //atualiza_mascara();
    //this->mascara = {{this->getX()-1.0f,this->getY()-1.0f,this->getZ()-1.0f},
    //        {this->getX()+1.0f,this->getY()+1.0f,this->getZ()+1.0f}};
    //this->mascara = {{this->getX(),this->getY(),this->getZ()},1.0f};
}