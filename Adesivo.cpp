// Código para Entidades que são adesivos
#include "Adesivo.h"
#include "Entidade.h"
#include "Draw.h"

Adesivo::Adesivo() : Entidade(), normal({0.0f,1.0f,0.0f}) {}
Adesivo::Adesivo(float ix, float iy, float iz, XYZ n)
: Entidade(ix, iy, iz), normal(n) {}

XYZ Adesivo::getNormal() const {return this->normal;}

void Adesivo::setNormal(XYZ n) {this->normal = n;}

void Adesivo::desenha_adesivo(){
    muda_cor(10);
    glPushMatrix();
    glTranslatef(this->getX(),this->getY(),this->getZ());
    glScalef(0.25f,0.25f,0.25f);
    desenha_cilindro();
    glPopMatrix();
}