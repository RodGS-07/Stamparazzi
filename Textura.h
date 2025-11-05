#ifndef TEXTURA_H
#define TEXTURA_H

#include "Linear.h"
#include "stb_image.h"
#include <GL/glut.h>
#include <cmath>

#define QTDTEXT 36 // quantidade de texturas

extern GLuint texID[QTDTEXT]; // ID para as texturas

extern const char* textureFileNames[QTDTEXT]; // nomes dos arquivos das texturas

void drawSquare(XYZ s,XYZ t);
void drawSphere(GLfloat r);
void CarregaTexturas();
void TransformaParede(int i, int s);
void DesenhaSkybox();
void desenha_paredes();
void desenha_chao();

#endif