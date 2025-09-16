// Código para o Jogador
#include "Linear.h"
#include "Colisao.h"
#include "Entidade.h"
#include "Draw.h"
#include "Adesivo.h"
#include "Poligono.h"

namespace NJ{

    class Jogador : public Entidade{
        public:
            float cam_yaw, cam_pitch;
            bool morto;
            //AABB mascara; //Sphere mascara;

            Jogador(float ix, float iy, float iz, float cy, float cp)
            : Entidade(ix,iy,iz), cam_yaw(cy), cam_pitch(cp), morto(false)/*,
                mascara({{this->pos.x-1.0f,this->pos.y-1.0f,this->pos.z-1.0f},
                        {this->pos.x+1.0f,this->pos.y+1.0f,this->pos.z+1.0f}})*/ { }
            
            Jogador(){};

            /*void atualiza_mascara() {
                this->mascara = {
                    { this->pos.x - 1.0f, this->pos.y - 1.0f, this->pos.z - 1.0f },
                    { this->pos.x + 1.0f, this->pos.y + 1.0f, this->pos.z + 1.0f }
                };
            }*/

            AABB getMascara() const {
                return {
                    { this->pos.x - 1.0f, this->pos.y - 1.0f, this->pos.z - 1.0f },
                    { this->pos.x + 1.0f, this->pos.y + 1.0f, this->pos.z + 1.0f }
                };
            }

            void nasce_jogador(float ix, float iy, float iz){
                this->pos = {ix, iy, iz};
                this->cam_yaw = this->cam_pitch = 0.0f;
                //atualiza_mascara();
                morto = false;
            }

            void desenha_mascara(int stacks = 30, int fatias = 30){
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
                        float x1 = this->mascara.r * cos(phi2) * sin(theta1) + this->pos.x;
                        float y1 = this->mascara.r * sin(phi2) + this->pos.y;
                        float z1 = this->mascara.r * cos(phi2) * cos(theta1) + this->pos.z;
                        glVertex3f(x1, y1, z1);

                        // Vertex 2 (bottom-right of current quad)
                        float x2 = this->mascara.r * cos(phi2) * sin(theta2) + this->pos.x;
                        float y2 = this->mascara.r * sin(phi2) + this->pos.y;
                        float z2 = this->mascara.r * cos(phi2) * cos(theta2) + this->pos.z;
                        glVertex3f(x2, y2, z2);

                        // Vertex 3 (top-right of current quad)
                        float x3 = this->mascara.r * cos(phi1) * sin(theta2) + this->pos.x;
                        float y3 = this->mascara.r * sin(phi1) + this->pos.y;
                        float z3 = this->mascara.r * cos(phi1) * cos(theta2) + this->pos.z;
                        glVertex3f(x3, y3, z3);

                        // Vertex 4 (top-left of current quad)
                        float x4 = this->mascara.r * cos(phi1) * sin(theta1) + this->pos.x;
                        float y4 = this->mascara.r * sin(phi1) + this->pos.y;
                        float z4 = this->mascara.r * cos(phi1) * cos(theta1) + this->pos.z;
                        glVertex3f(x4, y4, z4);
                    }
                glEnd();
                }*/
            }

            void desenha_mira(){
                float radYaw   = cam_yaw   * M_PI / 180.0f;
                float radPitch = cam_pitch * M_PI / 180.0f;
                float dirX = -sin(radYaw) * cos(radPitch);
                float dirY =  sin(radPitch);
                float dirZ = -cos(radYaw) * cos(radPitch);

                muda_cor(6);
                //glLineWidth(5.0f);
                glBegin(GL_LINES);
                glVertex3f(this->pos.x,this->pos.y,this->pos.z);
                glVertex3f(this->pos.x+dirX*1000.0f,this->pos.y+dirY*1000.0f,this->pos.z+dirZ*1000.0f);
                glEnd();
                //glLineWidth(1.0f);
            }

            bool detecta_adesivo(const Adesivo& a){
                float radYaw   = cam_yaw   * M_PI / 180.0f;
                float radPitch = cam_pitch * M_PI / 180.0f;
                float dirX = -sin(radYaw) * cos(radPitch);
                float dirY =  sin(radPitch);
                float dirZ = -cos(radYaw) * cos(radPitch);
                XYZ vi = {this->pos.x,this->pos.y,this->pos.z},
                    vf = {this->pos.x+dirX,this->pos.y+dirY,this->pos.z+dirZ},
                    va = {a.getX(),a.getY(),a.getZ()};
                float grau = Arccos((vf-vi),(va-vi)) * 180.0f / M_PI;
                return grau <= 20.0f and distancia_entidades(*this,a) <= 30.0f;
            }

            void tirou_foto(const Adesivo& a){
                if (flash_ativo and detecta_adesivo(a)) {
                    glDisable(GL_DEPTH_TEST);
                    glMatrixMode(GL_PROJECTION);
                    glPushMatrix();
                    glLoadIdentity();
                    glOrtho(0, 800, 600, 0, -1, 1);

                    glMatrixMode(GL_MODELVIEW);
                    glPushMatrix();
                    glLoadIdentity();

                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glColor4f(1.0f, 1.0f, 1.0f, flash_alpha);

                    glBegin(GL_QUADS);
                        glVertex2f(0, 0);
                        glVertex2f(800, 0);
                        glVertex2f(800, 600);
                        glVertex2f(0, 600);
                    glEnd();

                    glDisable(GL_BLEND);

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

            bool tenta_mover(float dx, float dy, float dz){
                candidate = {
                    { pos.x - 1.0f + dx, pos.y - 1.0f + dy, pos.z - 1.0f + dz },
                    { pos.x + 1.0f + dx, pos.y + 1.0f + dy, pos.z + 1.0f + dz }
                };
                //this->mascara; //Sphere candidate = this->mascara;
                //candidate.min.x += dx; candidate.max.x += dx;
                //candidate.min.y += dy; candidate.max.y += dy;
                //candidate.min.z += dz; candidate.max.z += dz;
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
                    if (p->colide_jogador(candidate))
                        return false; // colisão detectada => rejeita movimento
                    
                // sem colisão => confirma movimento
                this->pos.x += dx;
                this->pos.y += dy;
                this->pos.z += dz;
                //touch = candidate;
                //atualiza_mascara();
                //this->mascara = candidate;
                return true;
            }

            void prende_camera(){
                if(cam_yaw < 0.0f) cam_yaw += 360.0f;
                if(cam_yaw > 360.0f) cam_yaw -= 360.0f;
                if(cam_pitch > 90.0f) cam_pitch = 90.0f;
                if(cam_pitch < -90.0f) cam_pitch = -90.0f;
            }

            void move_camera(float dist, float dir, float val = 0.0f){
                if(dir >= 0.0f){
                    float rad = (cam_yaw + dir) * M_PI / 180.0f;
                    float dx = - sin(rad) * dist * dt;
                    float dz = - cos(rad) * dist * dt;
                    tenta_mover(dx,0.0f,dz);
                } else {
                    float dy = dist * val * dt;
                    tenta_mover(0.0f,dy,0.0f);
                }
                
            }

            void controle_camera(float move_vel, float camera_sens){
                if(!pause and !game_controller){
                    int midx = 320, midy = 240, tempx, tempy;
                    SDL_ShowCursor(SDL_DISABLE);
                    SDL_GetMouseState(&tempx, &tempy);
                    cam_yaw += camera_sens * (midx - tempx) * dt;
                    cam_pitch += camera_sens * (midy - tempy) * dt;
                    prende_camera();
                    SDL_WarpMouseInWindow(window,midx,midy);
                    state = SDL_GetKeyboardState(NULL);
                    if(state[SDL_SCANCODE_UP] or state[SDL_SCANCODE_W])
                        if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                            move_camera(move_vel,0.0f);
                    if(state[SDL_SCANCODE_DOWN] or state[SDL_SCANCODE_S])
                        if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                            move_camera(move_vel,180.0f);
                    if(state[SDL_SCANCODE_LEFT] or state[SDL_SCANCODE_A])
                        move_camera(move_vel,90.0f);
                    if(state[SDL_SCANCODE_RIGHT] or state[SDL_SCANCODE_D])
                        move_camera(move_vel,270.0f);
                    if(state[SDL_SCANCODE_LSHIFT] or state[SDL_SCANCODE_RSHIFT])
                        move_camera(move_vel,-1.0f,1.0f);
                    if(state[SDL_SCANCODE_LCTRL] or state[SDL_SCANCODE_RCTRL])
                        move_camera(move_vel,-1.0f,-1.0f);
                } else if(!pause and game_controller) {
                    Sint16 axisRX = SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_RIGHTX);
                    Sint16 axisRY = SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_RIGHTY);
                    if(fabs(axisRX) > 16000.0f) cam_yaw   -= (static_cast<float>(axisRX) / 32767.0f) * ANALOG_SENS;  // multiplica para sensibilidade
                    if(fabs(axisRY) > 16000.0f) cam_pitch -= (static_cast<float>(axisRY) / 32767.0f) * ANALOG_SENS;
                    prende_camera();
                    if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_UP)
                        or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTY) < -16000)
                        if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                            move_camera(move_vel,0.0f);
                    if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN)
                        or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTY) > 16000)
                        if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                            move_camera(move_vel,180.0f);
                    if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT)
                        or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTX) < -16000)
                        move_camera(move_vel,90.0f);
                    if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
                        or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTX) > 16000)
                        move_camera(move_vel,270.0f);
                    if(SDL_GameControllerGetButton(game_controller,SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
                        move_camera(move_vel,-1.0f,1.0f);
                    if(SDL_GameControllerGetButton(game_controller,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
                        move_camera(move_vel,-1.0f,-1.0f);
                }
                //atualiza_mascara();
                //this->mascara = {{this->pos.x-1.0f,this->pos.y-1.0f,this->pos.z-1.0f},
                //        {this->pos.x+1.0f,this->pos.y+1.0f,this->pos.z+1.0f}};
                //this->mascara = {{this->pos.x,this->pos.y,this->pos.z},1.0f};
            }
    };
};
