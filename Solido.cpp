// Códigos para entidades que são Polígonos
#include "Solido.h"
#include "Entidade.h"
#include "Draw.h"
#include "Colisao.h"
#include "Adesivo.h"
#include "Jogador.h"
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <memory>
using namespace std;

Solido::Solido(int s) : Entidade(), superficie(s) {}
Solido::Solido(float ix, float iy, float iz, int s, unique_ptr<Adesivo> a) 
: Entidade(ix,iy,iz), superficie(s) {
    this->setAdesivo(move(a));
}
Solido::Solido(float ix, float iy, float iz, int op, float xs, float ys, float zs, int s, unique_ptr<Adesivo> a)
: Entidade(ix,iy,iz), superficie(s) {
    if(!op) {rotx = xs, roty = ys, rotz = zs;} 
    else {escalax = xs, escalay = ys, escalaz = zs;}
    this->setAdesivo(move(a));
    if(adesivo) {
        XYZ normal = adesivo->getNormal();

        float rx = rotx * M_PI / 180.0f;
        float ry = roty * M_PI / 180.0f;
        float rz = rotz * M_PI / 180.0f;

        // A ordem das rotações deve ser a mesma que você usa para desenhar!
        if(rotx!=0.0f) normal = rotX(normal, rx);
        if(roty!=0.0f) normal = rotY(normal, ry);
        if(rotz!=0.0f) normal = rotZ(normal, rz);

        adesivo->setNormal(normal);

        //if(rotx!=0.0f) adesivo->setNormal(rotX(adesivo->getNormal(), rotx));
        //if(roty!=0.0f) adesivo->setNormal(rotY(adesivo->getNormal(), roty));
        //if(rotz!=0.0f) adesivo->setNormal(rotZ(adesivo->getNormal(), rotz));
    }
}

int Solido::getSuperficie() const {return this->superficie;}

float Solido::getEscalaX() const {return this->escalax;}

float Solido::getEscalaY() const {return this->escalay;}

float Solido::getEscalaZ() const {return this->escalaz;}

float Solido::getRotX() const {return this->rotx;}

float Solido::getRotY() const {return this->roty;}

float Solido::getRotZ() const {return this->rotz;}

void Solido::setEscala(float xs, float ys, float zs) {
    escalax = xs, escalay = ys, escalaz = zs;
}

Adesivo* Solido::getAdesivo() const {return adesivo.get();}

void Solido::setAdesivo(unique_ptr<Adesivo> a) {adesivo = move(a);}

void Solido::desenha_adesivo_no_solido(const Adesivo& adesivo, float offset) {
    glPushMatrix();

    // pega normal da face
    auto n = adesivo.getNormal();

    // rotaciona conforme a normal
    //if (n.x == 1)       glRotatef(90, 0, 1, 0);   // X+
    //else if (n.x == -1) glRotatef(-90, 0, 1, 0);  // X-
    //else if (n.y == 1)  glRotatef(-90, 1, 0, 0);  // Y+
    //else if (n.y == -1) glRotatef(90, 1, 0, 0);   // Y-
    //else if (n.z == -1) glRotatef(180, 0, 1, 0);  // Z-
    // se for Z+ (default), não precisa girar

    // empurra até a superfície do polígono
    //glTranslatef(n.x * offset, n.y * offset, n.z * offset);

    // desenha o adesivo plano
    adesivo.desenha_adesivo();

    glPopMatrix();
}

Cubo::Cubo() : Solido(F::CUBO) {}
Cubo::Cubo(float ix, float iy, float iz, unique_ptr<Adesivo> a, float l)
: Solido(ix,iy,iz,F::CUBO,move(a)), lado(l) {}
Cubo::Cubo(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float l)
: Solido(ix,iy,iz,op,xs,ys,zs,F::CUBO,move(a)), lado(l) {}

AABB Cubo::getAABB() const {
    return {{ (this->getX() - lado) * getEscalaX(), (this->getY() - lado) * getEscalaY(), (this->getZ() - lado) * getEscalaZ() },
            { (this->getX() + lado) * getEscalaX(), (this->getY() + lado) * getEscalaY(), (this->getZ() + lado) * getEscalaZ()}};
}

float Cubo::getLado() const {return this->lado;}

void Cubo::realiza_movimento(XYZ j, float dt, float dist, bool pause, bool modo_daltonico) {
    //desenha_solido(cor, pause, modo_daltonico);
    // if (getAdesivo() and (getRotX() or getRotY() or getRotZ())) {

    //     XYZ normal;

    //     float rx = getRotX() * M_PI / 180.0f;
    //     float ry = getRotY() * M_PI / 180.0f;
    //     float rz = getRotZ() * M_PI / 180.0f;

    //     // A ordem das rotações deve ser a mesma que você usa para desenhar!
    //     if(getRotX()) normal = rotX(normal, rx);
    //     if(getRotY()) normal = rotY(normal, ry);
    //     if(getRotZ()) normal = rotZ(normal, rz);

    //     getAdesivo()->setNormal(normal);
    // }
}

bool Cubo::colide_jogador(const AABB& s) const {
    AABB box = getAABB(); //{{this->getX() - lado, this->getY() - lado, this->getZ() - lado}, {this->getX() + lado, this->getY() + lado, this->getZ() + lado}};
    return AABBvsAABB(s, box);//SphereVsAABB(s,box);
}

void Cubo::aplica_efeito(Jogador& jogador, int& vidas) {
    return;
}

void Cubo::desenha_solido(int cor, bool pause, bool modo_daltonico) {
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();
    glTranslatef(this->getX(), this->getY(), this->getZ());
    if(getRotX()) glRotatef(getRotX(), 1, 0, 0);
    if(getRotY()) glRotatef(getRotY(), 0, 1, 0);
    if(getRotZ()) glRotatef(getRotZ(), 0, 0, 1);
    // aqui `lado` é tratado como meio-extent (compatível com sua desenha_cubo)
    desenha_cubo(this->lado,this->getAdesivo()->getTexturaID(), modo_daltonico);
    // Desenha adesivo colado na face +Z
    // if (this->getAdesivo()) {
    //     glPushMatrix();
    //     glTranslatef(0.0f, 0.0f, this->getLado()/2.0f + 0.01f);
    //     desenha_adesivo_no_solido(*this->getAdesivo(), this->getLado());
    //     glPopMatrix();
    // }
    glPopMatrix();
}

// void Cubo::desenha_adesivo() {
//     glPushMatrix();
//     glTranslatef(this->getX(), this->getY(), this->getZ());
//     desenha_adesivo_no_solido(this->getAdesivo(), this->getLado()/2.0f + 0.01f);
//     glPopMatrix();
// }

void Cubo::desenha_mascara() {
    muda_cor(12);
    AABB mascara = getAABB();//{{this->getX() - lado, this->getY() - lado, this->getZ() - lado}, {this->getX() + lado, this->getY() + lado, this->getZ() + lado}};

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
}

void Cubo::finaliza_som() {
    return;
}

Piramide::Piramide() : Solido(F::PIRAMIDE) {}
Piramide::Piramide(float ix, float iy, float iz, unique_ptr<Adesivo> a, float b, float h)
: Solido(ix,iy,iz,F::PIRAMIDE,move(a)), base(b), altura(h) {}
Piramide::Piramide(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float b, float h)
: Solido(ix,iy,iz,op,xs,ys,zs,F::PIRAMIDE,move(a)), base(b), altura(h) {}

AABB Piramide::getAABB() const {
    return {{(this->getX() - base/2.0f) * getEscalaX(), (this->getY() - altura/2.0f) * getEscalaY(), (this->getZ() - base/2.0f) * getEscalaZ()}, 
            {(this->getX() + base/2.0f) * getEscalaX(), (this->getY() + altura/2.0f) * getEscalaY(), (this->getZ() + base/2.0f) * getEscalaZ()}};
}

float Piramide::getAltura() const {return this->altura;}

void Piramide::realiza_movimento(XYZ j, float dt, float dist, bool pause, bool modo_daltonico) {
    //desenha_solido(cor, pause, modo_daltonico);
    // if (getAdesivo() and (getRotX() or getRotY() or getRotZ())) {

    //     XYZ normal;

    //     float rx = getRotX() * M_PI / 180.0f;
    //     float ry = getRotY() * M_PI / 180.0f;
    //     float rz = getRotZ() * M_PI / 180.0f;

    //     // A ordem das rotações deve ser a mesma que você usa para desenhar!
    //     if(getRotX()) normal = rotX(normal, rx);
    //     if(getRotY()) normal = rotY(normal, ry);
    //     if(getRotZ()) normal = rotZ(normal, rz);

    //     getAdesivo()->setNormal(normal);
    // }
}

bool Piramide::colide_jogador(const AABB& s) const {
    float b = base / 2.0f;
    float h = altura / 2.0f;
    AABB box = getAABB();//{{(this->getX() - b) * getEscalaX(), (this->getY() - h) * getEscalaY(), (this->getZ() - b) * getEscalaZ()}, 
               // {(this->getX() + b) * getEscalaX(), (this->getY() + h) * getEscalaY(), (this->getZ() + b) * getEscalaZ()}};
    //if(!AABBvsAABB(s,box)) {
    //cout << s.min.x << " " << s.min.y << " " << s.min.z << " " << s.max.x << " " << s.max.y << " " << s.max.z << endl;
    //cout << box.min.x << " " << box.min.y << " " << box.min.z << " " << box.max.x << " " << box.max.y << " " << box.max.z << endl;
    //}
    return AABBvsAABB(s,box);//SphereVsAABB(s, box);
    // base quad vertices (conforme ND::desenha_piramide)
    /*XYZ A = { this->getX() - b, this->getY() - b, this->getZ() - b };
    XYZ B = { this->getX() + b, this->getY() - b, this->getZ() - b };
    XYZ C = { this->getX() + b, this->getY() - b, this->getZ() + b };
    XYZ D = { this->getX() - b, this->getY() - b, this->getZ() + b };

    // apex (conforme ND::desenha_piramide: y = h - b)
    XYZ Apex = { this->getX(), this->getY() + (altura - b), this->getZ() };

    // base split into two triangles (A,B,C) and (A,C,D)
    if (SphereVsTriangle(s, A, B, C)) return true;
    if (SphereVsTriangle(s, A, C, D)) return true;

    // four lateral triangles
    if (SphereVsTriangle(s, A, B, Apex)) return true;
    if (SphereVsTriangle(s, B, C, Apex)) return true;
    if (SphereVsTriangle(s, C, D, Apex)) return true;
    if (SphereVsTriangle(s, D, A, Apex)) return true;

    return false;*/
}

void Piramide::aplica_efeito(Jogador& jogador, int& vidas) {
    jogador.morre(vidas);
}

void Piramide::desenha_solido(int cor, bool pause, bool modo_daltonico) {
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();
    glTranslatef(this->getX(), this->getY(), this->getZ());
    if(getRotX()) glRotatef(getRotX(), 1, 0, 0);
    if(getRotY()) glRotatef(getRotY(), 0, 1, 0);
    if(getRotZ()) glRotatef(getRotZ(), 0, 0, 1);
    desenha_piramide(this->base, this->altura,this->getAdesivo()->getTexturaID(), modo_daltonico);
    // if(this->getAdesivo()){
    //     glPushMatrix();
    //     glTranslatef(0.0f, this->getAltura()/2.0f, 1.0f);

    //     // Exemplo: adesivo na base
    //     //glTranslatef(0.0f, getAltura() / 2.0f, 0.0f);

    //     //Adesivo adesivo(0,0,0,{1,1,1}); 
    //     desenha_adesivo_no_solido(*this->getAdesivo(), this->getAltura()/2.0f + 0.01f);
    //     glPopMatrix();
    // }
    glPopMatrix();
}

// void Piramide::desenha_adesivo() {
//     glPushMatrix();
//     glTranslatef(this->getX(), this->getY() + this->getAltura()/2.0f, this->getZ());

//     // Exemplo: adesivo na base
//     //glTranslatef(0.0f, getAltura() / 2.0f, 0.0f);

//     //Adesivo adesivo(0,0,0,{1,1,1}); 
//     desenha_adesivo_no_solido(this->getAdesivo(), this->getAltura()/2.0f + 0.01f);

//     glPopMatrix();
// }

void Piramide::desenha_mascara(){
    muda_cor(12);
    float b = base / 2.0f, h = altura / 2.0f;
    AABB mascara = {{(this->getX() - b) * getEscalaX(), (this->getY() - h) * getEscalaY(), (this->getZ() - b) * getEscalaZ()}, 
                    {(this->getX() + b) * getEscalaX(), (this->getY() + h) * getEscalaY(), (this->getZ() + b) * getEscalaZ()}};
    //AABB mascara = {{this->getX() - b, this->getY() - h, this->getZ() - b}, {this->getX() + b, this->getY() + h, this->getZ() + b}};
    
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
}

void Piramide::finaliza_som() {
    return;
}

Esfera::Esfera() : Solido(F::ESFERA) {}
Esfera::Esfera(float ix, float iy, float iz, unique_ptr<Adesivo> a, float r, float c)
: Solido(ix,iy,iz,F::ESFERA,move(a)), raio(r), chao(c) {
    //y_vel = 10.0f;
    grav = -9.8f;     // "gravidade"
    altura_inicial = iy;

    // velocidade para chegar até a altura inicial
    velocidade_inicial = sqrt(2.0f * (-grav) * (altura_inicial - chao));

    y_vel = -velocidade_inicial;
    //chao = -1.0f;         // altura do chão (pode ser o y=-1 do seu cenário)
}

Esfera::Esfera(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float r, float c)
: Solido(ix,iy,iz,op,xs,ys,zs,F::ESFERA,move(a)), raio(r), chao(c) {
    //y_vel = 10.0f;
    grav = -9.8f;     // "gravidade"
    altura_inicial = iy;

    // velocidade para chegar até a altura inicial
    velocidade_inicial = sqrt(2.0f * (-grav) * (altura_inicial - chao));

    y_vel = -velocidade_inicial;
    //chao = -1.0f;         // altura do chão (pode ser o y=-1 do seu cenário)
}

AABB Esfera::getAABB() const {
    return {{ (this->getX() - raio) * getEscalaX(), (this->getY() - raio) * getEscalaY(), (this->getZ() - raio) * getEscalaZ() },
            { (this->getX() + raio) * getEscalaX(), (this->getY() + raio) * getEscalaY(), (this->getZ() + raio) * getEscalaZ()}};
}

float Esfera::getRaio() const {return this->raio;}

void Esfera::realiza_movimento(XYZ j, float dt, float dist, bool pause, bool modo_daltonico) {

    if(!pause){
        // Atualiza velocidade com gravidade
        y_vel += grav * dt;

        // Atualiza posição
        this->setY(this->getY() + y_vel * dt);

        // Checa colisão com o chão
        if (this->getY() - raio <= chao)
        {
            //cout << this->getAdesivo()->getTexturaID() << " colidiu com chao" << endl;
            this->setY(chao + raio);
            y_vel = velocidade_inicial; // sempre restaura a energia total
            if(dist <= 25.0f) Mix_PlayChannel(-1, som_batida, 0);
        }
        // if (this->getY() - raio <= chao) {
        //     this->setY(chao + raio);
        //     if (y_vel < 0) {                  // só inverte se estiver descendo
        //         y_vel = -y_vel;
        //     }
        // }

        if (this->getAdesivo()) {
            this->getAdesivo()->setX(this->getX());
            this->getAdesivo()->setY(this->getY());
            this->getAdesivo()->setZ(this->getZ());

            // if (getRotX() or getRotY() or getRotZ()) {
            //     XYZ normal;

            //     float rx = getRotX() * M_PI / 180.0f;
            //     float ry = getRotY() * M_PI / 180.0f;
            //     float rz = getRotZ() * M_PI / 180.0f;

            //     // A ordem das rotações deve ser a mesma que você usa para desenhar!
            //     if(getRotX()) normal = rotX(normal, rx);
            //     if(getRotY()) normal = rotY(normal, ry);
            //     if(getRotZ()) normal = rotZ(normal, rz);

            //     getAdesivo()->setNormal(normal);
            // }
        }
    }
    //desenha_solido(cor, pause, modo_daltonico);
}

bool Esfera::colide_jogador(const AABB& s) const {
    AABB box = getAABB();//{{this->getX() - raio, this->getY() - raio, this->getZ() - raio}, {this->getX() + raio, this->getY() + raio, this->getZ() + raio}};
    return AABBvsAABB(s,box);//SphereVsAABB(s, box);
    //Sphere s2 = {{this->getX(), this->getY(), this->getZ()}, raio };
    //return SphereVsSphere(s, s2);
}

void Esfera::aplica_efeito(Jogador& jogador, int& vidas) {
    AABB box = {{this->getX() - raio, this->getY() - raio, this->getZ() - raio}, {this->getX() + raio, this->getY() + raio, this->getZ() + raio}};
    //if(!pause) cout << jogador.getMascara().max.y << " " << box.min.y << endl;
    if(jogador.getMascara().max.y <= box.min.y or jogador.getMascara().min.y <= this->chao) {
        jogador.morre(vidas);
    }
}

void Esfera::desenha_solido(int cor, bool pause, bool modo_daltonico) {
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();
    glTranslatef(this->getX(), this->getY(), this->getZ());
    if(getRotX()) glRotatef(getRotX(), 1, 0, 0);
    if(getRotY()) glRotatef(getRotY(), 0, 1, 0);
    if(getRotZ()) glRotatef(getRotZ(), 0, 0, 1);
    desenha_esfera(this->raio, 30, 30, this->getAdesivo()->getTexturaID(), modo_daltonico);
    // if(this->getAdesivo()){
    //     glPushMatrix();
    //     glTranslatef(0.0f, 0.0f, this->getRaio()/2.0f + 0.01f);
    //     desenha_adesivo_no_solido(*this->getAdesivo(), this->getRaio() + 0.01f);
    //     glPopMatrix();
    // }
    glPopMatrix();
}

// void Esfera::desenha_adesivo() {
//     glPushMatrix();
//     glTranslatef(this->getX(), this->getY(), this->getZ());
//     desenha_adesivo_no_solido(this->getAdesivo(), this->getRaio() + 0.01f);
//     glPopMatrix();
// }

void Esfera::desenha_mascara(){
    muda_cor(12);
    AABB mascara = getAABB();//{{this->getX() - raio, this->getY() - raio, this->getZ() - raio}, {this->getX() + raio, this->getY() + raio, this->getZ() + raio}};
    
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
}

void Esfera::finaliza_som() {
    Mix_FreeChunk(som_batida);
}

Cilindro::Cilindro() : Solido(F::CILINDRO) {}
Cilindro::Cilindro(float ix, float iy, float iz, unique_ptr<Adesivo> a, float r, float h)
: Solido(ix,iy,iz,F::CILINDRO,move(a)), raio(r), altura(h), x_vel(1.0f) {
    centro_base = { this->getX(), this->getY(), this->getZ() - altura/2.0f };
    axis = { 0.0f, 0.0f, 1.0f };
    ang = 0.0f;
}
Cilindro::Cilindro(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float r, float h)
: Solido(ix,iy,iz,op,xs,ys,zs,F::CILINDRO,move(a)), raio(r), altura(h), x_vel(1.0f) {
    centro_base = { this->getX(), this->getY(), this->getZ() - altura/2.0f };
    axis = { 0.0f, 0.0f, 1.0f };
    ang = 0.0f;
}

AABB Cilindro::getAABB() const {
    float half = altura / 2.0f;
    return {{ (this->getX() - raio) * getEscalaX(), (this->getY() - raio) * getEscalaY(), (this->getZ() - half) * getEscalaZ()},
            { (this->getX() + raio) * getEscalaX(), (this->getY() + raio) * getEscalaY(), (this->getZ() + half) * getEscalaZ()}};
}

float Cilindro::getRaio() const {return this->raio;}

float Cilindro::getAltura() const {return this->altura;}

void Cilindro::realiza_movimento(XYZ j, float dt, float dist, bool pause, bool modo_daltonico) {
    float novaX = this->getX() + dt * 10.0f * x_vel;

    // Verifica se ultrapassou a borda
    if (novaX > (100.0f - raio)) {
        novaX = (100.0f - raio); // corrige posição
        x_vel = -1.0f;          // inverte direção
    }
    else if (novaX < -(100.0f - raio)) {
        novaX = -(100.0f - raio);
        x_vel = 1.0f;
    }

    if(!pause) {
        this->setX(novaX);

        // ============================
        // CONTROLE DO SOM DO ROLO
        // ============================
        if (dist <= 25.0f) {
            // só toca se não estiver tocando
            if (canal_rolo == -1 || !Mix_Playing(canal_rolo)) {
                canal_rolo = Mix_PlayChannel(-1, som_rolo, -1); // loop infinito
            }
        } else {
            // jogador saiu da área → para o som
            if (canal_rolo != -1) {
                Mix_HaltChannel(canal_rolo);
                canal_rolo = -1;
            }
        }

        // if(dist <= 25.0f) {
        //     Mix_SetPosition();
        // }
    } else {
        // jogador deu pause → para o som
        if (canal_rolo != -1) {
            Mix_HaltChannel(canal_rolo);
            canal_rolo = -1;
        }
    }

    if(!pause and this->getAdesivo()) {
        this->getAdesivo()->setX(this->getX());
        this->getAdesivo()->setY(this->getY());
        this->getAdesivo()->setZ(this->getZ());

        // if(getRotX() or getRotY() or getRotZ()) {
        //     XYZ normal;

        //     float rx = getRotX() * M_PI / 180.0f;
        //     float ry = getRotY() * M_PI / 180.0f;
        //     float rz = getRotZ() * M_PI / 180.0f;

        //     // A ordem das rotações deve ser a mesma que você usa para desenhar!
        //     if(getRotX()) normal = rotX(normal, rx);
        //     if(getRotY()) normal = rotY(normal, ry);
        //     if(getRotZ()) normal = rotZ(normal, rz);

        //     getAdesivo()->setNormal(normal);
        // }
    }
    // if (this->getAdesivo() and !pause) {
    //     auto n = this->getAdesivo()->getNormal();
    //     float rad = x_vel * M_PI / 180.0f;

    //     // Rotação da normal em torno do eixo Y (ou Z, conforme o cilindro no seu jogo)
    //     float nx = n.x * cos(rad) - n.z * sin(rad);
    //     float nz = n.x * sin(rad) + n.z * cos(rad);

    //     this->getAdesivo()->setNormal({nx, n.y, nz});
    // }

    //desenha_solido(cor, pause, modo_daltonico);
}

bool Cilindro::colide_jogador(const AABB& s) const {
    // AABB box = {
    //     { this->getX() - raio, this->getY() - raio, this->getZ() - altura/2.0f },
    //     { this->getX() + raio, this->getY() + raio, this->getZ() + altura/2.0f }
    // };
    AABB box = getAABB();
    return AABBvsAABB(s,box);//SphereVsAABB(s, box);
    /*Cylinder cyl;
    cyl.base = centro_base;    // use o centro_base calculado no construtor
    // garante axis unitário (aqui já é eixo Z, mas normalizar é seguro)
    float len = sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
    cyl.axis = { axis.x / (len ? len : 1.0f), axis.y / (len ? len : 1.0f), axis.z / (len ? len : 1.0f) };
    cyl.h = altura;
    cyl.R = raio;
    return SphereVsCylinder(s, cyl);*/
}

void Cilindro::aplica_efeito(Jogador& jogador, int& vidas) {
    AABB box = {{this->getX() - raio, this->getY() - raio, this->getZ() - raio}, {this->getX() + raio, this->getY() + raio, this->getZ() + raio}};
    //if(fabs(jogador.getX()) >= 100.0f) jogador.morre();
    //if(jogador.getMascara().max.y <= this->getY()) jogador.morre(vidas);
}

void Cilindro::desenha_solido(int cor, bool pause, bool modo_daltonico) {
    if(!pause) ang -= x_vel;
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();
    glTranslatef(this->getX(), this->getY(), this->getZ());
    /*if(!pause)*/ glRotatef(ang,0,0,1);
    desenha_cilindro(this->raio, this->altura, 30, 30, true, this->getAdesivo()->getTexturaID(), modo_daltonico);
    // if(this->getAdesivo()){
    //     glPushMatrix();
    //     glTranslatef(0.0f, 0.0f, this->getAltura()/2.0f + 0.01f);

    //     auto n = this->getAdesivo()->getNormal();
    //     float offset = (fabs(n.y) > 0) ? this->getAltura()/2.0f : this->getRaio();
    //     desenha_adesivo_no_solido(*this->getAdesivo(), offset + 0.01f);

    //     glPopMatrix();
    // }
    glPopMatrix();
}

// void Cilindro::desenha_adesivo() {
//     glPushMatrix();
//     glTranslatef(this->getX(), this->getY(), this->getZ());

//     auto n = this->getAdesivo().getNormal();
//     float offset = (fabs(n.y) > 0) ? this->getAltura()/2.0f : this->getRaio();
//     desenha_adesivo_no_solido(this->getAdesivo(), offset + 0.01f);

//     glPopMatrix();
// }

void Cilindro::desenha_mascara(){
    muda_cor(12);
    AABB mascara = getAABB();//{{this->getX() - raio, this->getY() - raio, this->getZ() - altura/2.0f}, {this->getX() + raio, this->getY() + raio, this->getZ() + altura/2.0f}};
    
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
}

void Cilindro::finaliza_som() {
    Mix_FreeChunk(som_rolo);
}

Cone::Cone() : Solido(F::CONE) {}
Cone::Cone(float ix, float iy, float iz, unique_ptr<Adesivo> a, float r, float h)
: Solido(ix,iy,iz,F::CONE,move(a)), raio(r), altura(h), ang(0.0f) { 
    apex = { this->getX(), this->getY(), this->getZ() };
    axis = { 0.0f, 0.0f, -1.0f }; 
}
Cone::Cone(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float r, float h)
: Solido(ix,iy,iz,op,xs,ys,zs,F::CONE,move(a)), raio(r), altura(h), ang(0.0f) { 
    apex = { this->getX(), this->getY(), this->getZ() };
    axis = { 0.0f, 0.0f, -1.0f }; 
}

AABB Cone::getAABB() const {
    return {
        { (this->getX() - raio) * getEscalaX(), (this->getY() - raio) * getEscalaY(), (this->getZ() - altura/2.0f) * getEscalaZ()},
        { (this->getX() + raio) * getEscalaX(), (this->getY() + raio) * getEscalaY(), (this->getZ() + altura/2.0f) * getEscalaZ()}
    };
}

float Cone::getRaio() const {return this->raio;}

float Cone::getAltura() const {return this->altura;}

void Cone::realiza_movimento(XYZ j, float dt, float dist, bool pause, bool modo_daltonico) {
    if(!pause){
        ang += 10.0f * dt;

        // rotaciona o eixo em torno do Y
        float cosA = cos(ang * M_PI/180.0f);
        float sinA = sin(ang * M_PI/180.0f);
        axis = { -sinA, 0.0f, -cosA };  // vetor rotacionado em Y

        // Atualiza ápice (sempre no topo do cone no mundo)
        apex = { this->getX(), this->getY(), this->getZ()};

        // -----------------------------
        // 2. Atualiza o Adesivo (posição + normal)
        // -----------------------------
        if (this->getAdesivo()) {
            this->getAdesivo()->setNormal({apex.x - axis.x*altura - apex.x,
                                            apex.y - axis.y*altura - apex.y,
                                            apex.z - axis.z*altura - apex.z});
        }

        // ============================
        // CONTROLE DO SOM DO LASER
        // ============================
        XYZ v = j - apex;
        XYZ cross = v * axis;
        float distLaser = !cross / !axis;
        if (distLaser <= 20.0f) {
            // só toca se não estiver tocando
            if (canal_laser == -1 || !Mix_Playing(canal_laser)) {
                canal_laser = Mix_PlayChannel(-1, som_laser, -1); // loop infinito
            }
        } else {
            // jogador saiu da área → para o som
            if (canal_laser != -1) {
                Mix_HaltChannel(canal_laser);
                canal_laser = -1;
            }
        }
        //if(dist <= 25.0f) Mix_PlayChannel(-1, som_laser, 0);
    } else {
        // jogador deu pause → para o som
        if (canal_laser != -1) {
            Mix_HaltChannel(canal_laser);
            canal_laser = -1;
        }
    }
    //desenha_solido(cor, pause, modo_daltonico);
}

bool Cone::colide_jogador(const AABB& s) const {
    // AABB box = {
    //     { this->getX() - raio, this->getY() - raio, this->getZ() - altura/2.0f },
    //     { this->getX() + raio, this->getY() + raio, this->getZ() + altura/2.0f }
    // };
    AABB box = getAABB();
    return AABBvsAABB(s,box);//SphereVsAABB(s, box);
    /*ConeBound cone;
    cone.apex = {this->getX(), this->getY(), this->getZ() + altura/2};  // ápice no topo
    cone.axis = {0,0,-1};  // apontando para baixo
    cone.h = altura;
    cone.R = raio;
    // Testa se o centro da esfera está dentro do cone expandido pelo raio da esfera
    return SphereVsCone(s,cone); //PointInConeBound(s.c, cone);*/
}

void Cone::aplica_efeito(Jogador& jogador, int& vidas) {
    AABB box = jogador.getMascara();

    // origem = ápice do cone
    XYZ origem = apex;
    // fim = ponto ao longo da direção do cone
    XYZ fim = apex - axis * altura * 50.0f; // 50x altura, laser longo

    if (SegmentVsAABB(origem, fim, box) and jogador.estaVivo()) {
        jogador.morre(vidas);
    }
}

void Cone::desenha_solido(int cor, bool pause, bool modo_daltonico) {
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();

        muda_cor(0);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex3f(apex.x, apex.y, apex.z);
            glVertex3f(apex.x - axis.x*altura*50,
                    apex.y - axis.y*altura*50,
                    apex.z - axis.z*altura*50);
        glEnd();
        glLineWidth(1.0f);
        muda_cor(cor);

        glTranslatef(this->getX(), this->getY(), this->getZ());
        glRotatef(ang,0,1,0);
        desenha_cone(this->raio, this->altura, 30, this->getAdesivo()->getTexturaID(), modo_daltonico);
        // if(this->getAdesivo()){
        //     glPushMatrix();
        //     glTranslatef(0.0f, this->getAltura()/2.0f+0.01f, 0.0f);

        //     auto n = this->getAdesivo()->getNormal();
        //     float offset = (fabs(n.y) > 0) ? this->getAltura()/2.0f : this->getRaio();
        //     desenha_adesivo_no_solido(*this->getAdesivo(), offset + 0.01f);

        //     glPopMatrix();
        // }

    glPopMatrix();
}

// void Cone::desenha_adesivo() {
//     glPushMatrix();
//     glTranslatef(this->getX(), this->getY(), this->getZ());

//     auto n = this->getAdesivo().getNormal();
//     float offset = (fabs(n.y) > 0) ? this->getAltura()/2.0f : this->getRaio();
//     desenha_adesivo_no_solido(this->getAdesivo(), offset + 0.01f);

//     glPopMatrix();
// }

void Cone::desenha_mascara(){
    muda_cor(12);
    AABB mascara = getAABB();//{{this->getX() - raio, this->getY() - raio, this->getZ() - altura/2.0f}, {this->getX() + raio, this->getY() + raio, this->getZ() + altura/2.0f}};
    
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
}

void Cone::finaliza_som() {
    Mix_FreeChunk(som_laser);
}

Torus::Torus() : Solido(F::TORUS) {}
Torus::Torus(float ix, float iy, float iz, unique_ptr<Adesivo> a, float re, float ra)
: Solido(ix,iy,iz,F::TORUS,move(a)) {
    p.c = {ix,iy,iz};
    p.raio_menor = re;
    p.raio_maior = ra;
    p.par = nullptr;
    conjugado = nullptr;
}
Torus::Torus(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float re, float ra)
: Solido(ix,iy,iz,op,xs,ys,zs,F::TORUS,move(a)) {
    p.c = {ix,iy,iz};
    p.raio_menor = re;
    p.raio_maior = ra;
    p.par = nullptr;
    conjugado = nullptr;
}

Torus* Torus::getConjugado() const {return this->conjugado;}

void Torus::setConjugado(Torus* t) {this->conjugado = t;}

AABB Torus::getAABB() const {
    return {
        {p.c.x - p.raio_maior, p.c.y - p.raio_maior, p.c.z - p.raio_maior},
        {p.c.x + p.raio_maior, p.c.y + p.raio_maior, p.c.z + p.raio_maior}
    };
}

void Torus::realiza_movimento(XYZ j, float dt, float dist, bool pause, bool modo_daltonico) {
    //desenha_solido(cor, pause, modo_daltonico);
    // if (getAdesivo() and (getRotX() or getRotY() or getRotZ())) {

    //     XYZ normal;

    //     float rx = getRotX() * M_PI / 180.0f;
    //     float ry = getRotY() * M_PI / 180.0f;
    //     float rz = getRotZ() * M_PI / 180.0f;

    //     // A ordem das rotações deve ser a mesma que você usa para desenhar!
    //     if(getRotX()) normal = rotX(normal, rx);
    //     if(getRotY()) normal = rotY(normal, ry);
    //     if(getRotZ()) normal = rotZ(normal, rz);

    //     getAdesivo()->setNormal(normal);
    // }
}

bool Torus::colide_jogador(const AABB& s) const{
    AABB box = {
        {p.c.x - p.raio_maior, p.c.y - p.raio_maior, p.c.z - p.raio_maior},
        {p.c.x + p.raio_maior, p.c.y + p.raio_maior, p.c.z + p.raio_maior}
    };
    return AABBvsAABB(s, box);
}

void Torus::aplica_efeito(Jogador& jogador, int& vidas) {
    //cout << (conjugado==NULL) << endl;
    bool in = fabs(jogador.getX()-this->getX()) <= p.raio_menor and fabs(jogador.getY()-this->getY()) <= p.raio_menor;
    if(conjugado and in){
        float dx = conjugado->getX();
        float dy = conjugado->getY();
        float dz = conjugado->getZ() - 5.0f * ((jogador.getZ() >= this->getZ()) ? 1.0f : -1.0f);
        //float dx = conjugado->p.c.x;// - p.c.x;
        //float dy = conjugado->p.c.y;// - p.c.y;
        //float dz = conjugado->p.c.z;// - p.c.z;

        // atualiza posição real do jogador
        jogador.setX(dx);
        jogador.setY(dy);
        jogador.setZ(dz);
        jogador.setMascara({{jogador.getX() - 1.0f, jogador.getY() - 1.0f, jogador.getZ() - 1.0f},
                            {jogador.getX() + 1.0f, jogador.getY() + 1.0f, jogador.getZ() + 1.0f}});
        
        Mix_PlayChannel(-1, som_teleporte, 0);
        
        // jogador.setX(jogador.getX() + dx);
        // jogador.setY(jogador.getY() + dy);
        // jogador.setZ(jogador.getZ() + dz);

        // atualiza a máscara com o mesmo deslocamento
        /*AABB mask = jogador.getMascara();
        mask.min.x += dx; mask.max.x += dx;
        mask.min.y += dy; mask.max.y += dy;
        mask.min.z += dz; mask.max.z += dz;
        jogador.setMascara(mask);*/
    }
    /*if(conjugado){
        // teleporta o jogador para o conjugado
        jogador.setMascara({{jogador.getMascara().min.x+(conjugado->p.c.x - p.c.x),
                                jogador.getMascara().min.y+(conjugado->p.c.y - p.c.y),
                                jogador.getMascara().min.z+(conjugado->p.c.z - p.c.z)},
                            {jogador.getMascara().max.x+(conjugado->p.c.x - p.c.x),
                                jogador.getMascara().max.y+(conjugado->p.c.y - p.c.y),
                                jogador.getMascara().max.z+(conjugado->p.c.z - p.c.z)}});
        jogador.getMascara().min.x += (conjugado->p.c.x - p.c.x);
        jogador.getMascara().max.x += (conjugado->p.c.x - p.c.x);
        jogador.getMascara().min.y += (conjugado->p.c.y - p.c.y);
        jogador.getMascara().max.y += (conjugado->p.c.y - p.c.y);
        jogador.getMascara().min.z += (conjugado->p.c.z - p.c.z);
        jogador.getMascara().max.z += (conjugado->p.c.z - p.c.z);
    }*/
}

void Torus::desenha_solido(int cor, bool pause, bool modo_daltonico) {
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();
    glTranslatef(p.c.x, p.c.y, p.c.z);
    if(getRotX()) glRotatef(getRotX(), 1, 0, 0);
    if(getRotY()) glRotatef(getRotY(), 0, 1, 0);
    if(getRotZ()) glRotatef(getRotZ(), 0, 0, 1);
    desenha_torus(p.raio_maior, p.raio_menor, 40, 40, this->getAdesivo()->getTexturaID(), modo_daltonico);
    // if(this->getAdesivo()){
    //     glPushMatrix();
    //     //glTranslatef(getX(), getY(), getZ());

    //     float offset = (this->p.raio_menor + this->p.raio_maior) / 2.0f;

    //     // Cola o adesivo no lado "frontal"
    //     glTranslatef(0.0f, 0.0f, offset);

    //     //Adesivo adesivo(0,0,0,{1,1,1}); 
    //     desenha_adesivo_no_solido(*this->getAdesivo(),offset);
    //     glPopMatrix();
    // }
    glPopMatrix();
}

// void Torus::desenha_adesivo() {
//     glPushMatrix();
//     glTranslatef(getX(), getY(), getZ());

//     float offset = (this->p.raio_menor + this->p.raio_maior) / 2.0f;

//     // Cola o adesivo no lado "frontal"
//     glTranslatef(0.0f, 0.0f, offset);

//     //Adesivo adesivo(0,0,0,{1,1,1}); 
//     desenha_adesivo_no_solido(this->getAdesivo(),offset);
// }

void Torus::desenha_mascara() {
    muda_cor(12);
    AABB mascara = {{this->getX() - p.raio_maior, this->getY() - p.raio_maior, this->getZ() - p.raio_maior}, {this->getX() + p.raio_maior, this->getY() + p.raio_maior, this->getZ() + p.raio_maior}};

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
}

void Torus::finaliza_som() {
    Mix_FreeChunk(som_teleporte);
}