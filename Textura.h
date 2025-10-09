#ifndef TEXTURA_H
#define TEXTURA_H

#include "Linear.h"
#include "stb_image.h"
#include <GL/glut.h>
#include <cmath>

// const int QTDTEXT; //quantidade de texturas
// GLuint texID[];  // Texture ID's for the three textures.

// char* textureFileNames[];   // file names for the files from which texture images are loaded

void drawSquare(XYZ s,XYZ t);
void drawSphere(GLfloat r);
void CarregaTexturas();
void TransformaParede(int i, int s);
void DesenhaSkybox();
void desenha_paredes();
void desenha_chao();

#endif