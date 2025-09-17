// Códigos para entidades que são Polígonos
#include "Poligono.h"
#include "Entidade.h"
#include "Draw.h"
#include "Colisao.h"

class Jogador;

Poligono::Poligono(int s) : Entidade(), superficie(s) {}
Poligono::Poligono(float ix, float iy, float iz, int s) 
: Entidade(ix,iy,iz), superficie(s) {}

Cubo::Cubo() : Poligono(F::CUBO) {}
Cubo::Cubo(float ix, float iy, float iz, float l)
: Poligono(ix,iy,iz,F::CUBO), lado(l) {}

bool Cubo::colide_jogador(const AABB& s) const {
    AABB box = {{this->getX() - lado, this->getY() - lado, this->getZ() - lado}, {this->getX() + lado, this->getY() + lado, this->getZ() + lado}};
    return AABBvsAABB(s, box);//SphereVsAABB(s,box);
}

void Cubo::aplica_efeito(Jogador& jogador) {
    return;
}

void Cubo::desenha_poligono(int cor) {
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();
    glTranslatef(this->getX(), this->getY(), this->getZ());
    // aqui `lado` é tratado como meio-extent (compatível com sua desenha_cubo)
    desenha_cubo(this->lado);
    glPopMatrix();
}

void Cubo::desenha_mascara() {
    muda_cor(12);
    AABB mascara = {{this->getX() - lado, this->getY() - lado, this->getZ() - lado}, {this->getX() + lado, this->getY() + lado, this->getZ() + lado}};

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

Piramide::Piramide() : Poligono(F::PIRAMIDE) {}
Piramide::Piramide(float ix, float iy, float iz, float b, float h)
: Poligono(ix,iy,iz,F::PIRAMIDE), base(b), altura(h) {}

bool Piramide::colide_jogador(const AABB& s) const {
    float b = base / 2.0f;
    float h = altura / 2.0f;
    AABB box = {{this->getX() - b, this->getY() - h, this->getZ() - b}, {this->getX() + b, this->getY() + h, this->getZ() + b}};
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

void Piramide::aplica_efeito(Jogador& jogador) {
    jogador.morre();
}

void Piramide::desenha_poligono(int cor) {
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();
    glTranslatef(this->getX(), this->getY(), this->getZ());
    desenha_piramide(this->base, this->altura);
    glPopMatrix();
}

void Piramide::desenha_mascara(){
    muda_cor(12);
    float b = base / 2.0f, h = altura / 2.0f;
    AABB mascara = {{this->getX() - b, this->getY() - h, this->getZ() - b}, {this->getX() + b, this->getY() + h, this->getZ() + b}};
    
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

Esfera::Esfera() : Poligono(F::ESFERA) {}
Esfera::Esfera(float ix, float iy, float iz, float r)
: Poligono(ix,iy,iz,F::ESFERA), raio(r) {}

bool Esfera::colide_jogador(const AABB& s) const {
    AABB box = {{this->getX() - raio, this->getY() - raio, this->getZ() - raio}, {this->getX() + raio, this->getY() + raio, this->getZ() + raio}};
    return AABBvsAABB(s,box);//SphereVsAABB(s, box);
    //Sphere s2 = {{this->getX(), this->getY(), this->getZ()}, raio };
    //return SphereVsSphere(s, s2);
}

void Esfera::aplica_efeito(Jogador& jogador) {
    AABB box = {{this->getX() - raio, this->getY() - raio, this->getZ() - raio}, {this->getX() + raio, this->getY() + raio, this->getZ() + raio}};
    //if(!pause) cout << jogador.getMascara().max.y << " " << box.min.y << endl;
    if(jogador.getMascara().max.y <= this->getY()) jogador.morre();
}

void Esfera::desenha_poligono(int cor) {
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();
    glTranslatef(this->getX(), this->getY(), this->getZ());
    desenha_esfera(this->raio, 30, 30);
    glPopMatrix();
}

void Esfera::desenha_mascara(){
    muda_cor(12);
    AABB mascara = {{this->getX() - raio, this->getY() - raio, this->getZ() - raio}, {this->getX() + raio, this->getY() + raio, this->getZ() + raio}};
    
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

Cilindro::Cilindro() : Poligono(F::CILINDRO) {}
Cilindro::Cilindro(float ix, float iy, float iz, float r, float h)
: Poligono(ix,iy,iz,F::CILINDRO), raio(r), altura(h) {
    centro_base = { this->getX(), this->getY(), this->getZ() - altura/2.0f };
    axis = { 0.0f, 0.0f, 1.0f };
}

bool Cilindro::colide_jogador(const AABB& s) const override {
    AABB box = {
        { this->getX() - raio, this->getY() - raio, this->getZ() - altura/2.0f },
        { this->getX() + raio, this->getY() + raio, this->getZ() + altura/2.0f }
    };
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

void Cilindro::aplica_efeito(Jogador& jogador) override {
    AABB box = {{this->getX() - raio, this->getY() - raio, this->getZ() - raio}, {this->getX() + raio, this->getY() + raio, this->getZ() + raio}};
    if(jogador.getMascara().max.y <= this->getY()) jogador.morre();
}

void Cilindro::desenha_poligono(int cor) override {
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();
    glTranslatef(this->getX(), this->getY(), this->getZ());
    desenha_cilindro(this->raio, this->altura, 30, 30, true);
    glPopMatrix();
}

void Cilindro::desenha_mascara(){
    muda_cor(12);
    AABB mascara = {{this->getX() - raio, this->getY() - raio, this->getZ() - altura/2.0f}, {this->getX() + raio, this->getY() + raio, this->getZ() + altura/2.0f}};
    
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

Cone::Cone() : Poligono(F::CONE) {}
Cone::Cone(float ix, float iy, float iz, float r, float h)
: Poligono(ix,iy,iz,F::CONE), raio(r), altura(h) { 
    apex = { this->getX(), this->getY(), this->getZ() + altura/2.0f };
    axis = { 0.0f, 0.0f, -1.0f }; 
}

bool Cone::colide_jogador(const AABB& s) const override {
    AABB box = {
        { this->getX() - raio, this->getY() - raio, this->getZ() - altura/2.0f },
        { this->getX() + raio, this->getY() + raio, this->getZ() + altura/2.0f }
    };
    return AABBvsAABB(s,box);//SphereVsAABB(s, box);
    /*ConeBound cone;
    cone.apex = {this->getX(), this->getY(), this->getZ() + altura/2};  // ápice no topo
    cone.axis = {0,0,-1};  // apontando para baixo
    cone.h = altura;
    cone.R = raio;
    // Testa se o centro da esfera está dentro do cone expandido pelo raio da esfera
    return SphereVsCone(s,cone); //PointInConeBound(s.c, cone);*/
}

void Cone::aplica_efeito(Jogador& jogador) override {
    AABB box = jogador.getMascara();

    // origem = ápice do cone
    XYZ origem = apex;
    // fim = ponto ao longo da direção do cone
    XYZ fim = apex - axis * altura * 30.0f; // 2x altura, laser longo

    if (SegmentVsAABB(origem, fim, box)) {
        jogador.morre();
    }
}

void Cone::desenha_poligono(int cor) override {
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();
    glTranslatef(this->getX(), this->getY(), this->getZ());
    desenha_cone(this->raio, this->altura, 30);
    glPopMatrix();

    muda_cor(0);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex3f(apex.x, apex.y, apex.z);
        glVertex3f(apex.x - axis.x*altura*30,
                apex.y - axis.y*altura*30,
                apex.z - axis.z*altura*30);
    glEnd();
    glLineWidth(1.0f);
    muda_cor(cor);
}

void Cone::desenha_mascara(){
    muda_cor(12);
    AABB mascara = {{this->getX() - raio, this->getY() - raio, this->getZ() - altura/2.0f}, {this->getX() + raio, this->getY() + raio, this->getZ() + altura/2.0f}};
    
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

Torus::Torus() : Poligono(F::TORUS) {}
Torus::Torus(float ix, float iy, float iz, float re, float ra)
: Poligono(ix,iy,iz,F::TORUS) {
    p.c = {ix,iy,iz};
    p.raio_menor = re;
    p.raio_maior = ra;
    p.par = nullptr;
    conjugado = nullptr;
}

bool Torus::colide_jogador(const AABB& s) const override{
    AABB box = {
        {p.c.x - p.raio_maior, p.c.y - p.raio_maior, p.c.z - p.raio_menor},
        {p.c.x + p.raio_maior, p.c.y + p.raio_maior, p.c.z + p.raio_menor}
    };
    return AABBvsAABB(s, box);
}

void Torus::aplica_efeito(Jogador& jogador) override {
    if(conjugado){
        // teleporta o jogador para o conjugado
        jogador.getMascara().min.x += (conjugado->p.c.x - p.c.x);
        jogador.getMascara().max.x += (conjugado->p.c.x - p.c.x);
        jogador.getMascara().min.y += (conjugado->p.c.y - p.c.y);
        jogador.getMascara().max.y += (conjugado->p.c.y - p.c.y);
        jogador.getMascara().min.z += (conjugado->p.c.z - p.c.z);
        jogador.getMascara().max.z += (conjugado->p.c.z - p.c.z);
    }
}

void Torus::desenha_poligono(int cor) override {
    if(cor >= 0 and cor <= 12) muda_cor(cor);
    glPushMatrix();
    glTranslatef(p.c.x, p.c.y, p.c.z);
    desenha_torus(p.raio_maior, p.raio_menor, 40, 40);
    glPopMatrix();
}

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