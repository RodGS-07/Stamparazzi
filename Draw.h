#ifndef DRAW_H
#define DRAW_H

#include "Linear.h"

enum F{
    CUBO,
    PIRAMIDE,
    ESFERA,
    CILINDRO,
    CONE,
    TORUS,
    BEZIER
};

const float cores[13][3] = {
    {1.0f,0.0f,0.0f}, //vermelho
    {1.0f,0.5f,0.0f}, //laranja
    {1.0f,1.0f,0.0f}, //amarelo
    {0.0f,1.0f,0.0f}, //lima
    {0.0f,0.5f,0.0f}, //verde
    {0.0f,1.0f,1.0f}, //ciano
    {0.0f,0.0f,1.0f}, //azul
    {0.5f,0.0f,1.0f}, //roxo
    {1.0f,0.0f,1.0f}, //rosa
    {0.5f,0.25f,0.0f}, //marrom
    {1.0f,1.0f,1.0f}, //branco
    {0.5f,0.5f,0.5f}, //cinza
    {0.0f,0.0f,0.0f} /*preto*/ };

void muda_cor(int c);
void desenha_chao();
void desenha_cubo(float lado = 2.0f);
void desenha_piramide(float base = 4.0f, float altura = 4.0f);
void desenha_esfera(float raio = 2.0f, int fatias = 30, int stacks = 30);
void desenha_cilindro(float raio = 2.0f, float altura = 4.0f, int fatias = 30, int stacks = 30, bool tampas = true);
void desenha_cone(float raio = 2.0f, float altura = 4.0f, int fatias = 30);
void desenha_torus(float R = 3.0f, float r = 1.0f, int fatias = 30, int stacks = 30);
void desenha_superficie(int formato);
void marcax(float x, float y, float z);

const int NI = 10, NJ = 10;
const int RESOLUTIONI = 3*NI, RESOLUTIONJ = 3*NJ;
XYZ inp[NI+1][NJ+1];
XYZ outp[RESOLUTIONI][RESOLUTIONJ];

double BezierBlend(int k,double mu, int n);
void entrada_inps(int forma, int i, int j);
void generateControlPoint(int forma);
XYZ calculaNormal(XYZ u, XYZ v);
void Surface(int forma);

#endif