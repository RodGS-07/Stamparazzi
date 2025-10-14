// Código para Entidades que são adesivos
#include "Adesivo.h"
#include "Entidade.h"
#include "Draw.h"
#include "Textura.h"

Adesivo::Adesivo() : Entidade(), normal({0.0f,0.0f,1.0f}) {}
Adesivo::Adesivo(float ix, float iy, float iz, int t, XYZ n)
: Entidade(ix, iy, iz), texturaID(3+t), normal(n) {}

int Adesivo::getTexturaID() const {return this->texturaID;}

void Adesivo::setTexturaID(int t) {texturaID = t+3;}

XYZ Adesivo::getNormal() const {return this->normal;}

void Adesivo::setNormal(XYZ n) {this->normal = n;}

void Adesivo::desenha_adesivo() const {
    float size = 1.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[texturaID]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1,1,1); // mantém as cores originais da textura

    glPushMatrix();

    // ajusta orientação da textura de acordo com a normal
    if (normal.x == 1) glRotatef(90, 0, 1, 0);
    else if (normal.x == -1) glRotatef(-90, 0, 1, 0);
    else if (normal.y == 1) glRotatef(-90, 1, 0, 0);
    else if (normal.y == -1) glRotatef(90, 1, 0, 0);
    else if (normal.z == -1) glRotatef(180, 0, 1, 0);

    glBegin(GL_QUADS);
        glNormal3f(normal.x, normal.y, normal.z);
        glTexCoord2f(0, 0); glVertex3f(-size/2, -size/2, 0);
        glTexCoord2f(1, 0); glVertex3f( size/2, -size/2, 0);
        glTexCoord2f(1, 1); glVertex3f( size/2,  size/2, 0);
        glTexCoord2f(0, 1); glVertex3f(-size/2,  size/2, 0);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    
    // float size = 0.5f;
    // muda_cor(10);

    // glBegin(GL_QUADS);
    //     glVertex3f(-size/2, -size/2, 0);
    //     glVertex3f( size/2, -size/2, 0);
    //     glVertex3f( size/2,  size/2, 0);
    //     glVertex3f(-size/2,  size/2, 0);
    // glEnd();

    // muda_cor(10);
    // glPushMatrix();
    // glTranslatef(this->getX(),this->getY(),this->getZ());
    // glScalef(0.25f,0.25f,0.25f);
    // desenha_cilindro();
    // glPopMatrix();
}