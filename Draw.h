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
    {1.0f,0.0f,0.0f}, //vermelho = 0
    {1.0f,0.5f,0.0f}, //laranja = 1
    {1.0f,1.0f,0.0f}, //amarelo = 2
    {0.0f,1.0f,0.0f}, //lima = 3
    {0.0f,0.5f,0.0f}, //verde = 4
    {0.0f,1.0f,1.0f}, //ciano = 5
    {0.0f,0.0f,1.0f}, //azul = 6
    {0.5f,0.0f,1.0f}, //roxo = 7
    {1.0f,0.0f,1.0f}, //rosa = 8
    {0.5f,0.25f,0.0f}, //marrom = 9
    {1.0f,1.0f,1.0f}, //branco = 10
    {0.5f,0.5f,0.5f}, //cinza = 11
    {0.0f,0.0f,0.0f} /*preto = 12*/ };

void muda_cor(int c);
void desenha_cubo(float lado = 2.0f, int id = 3);
void desenha_piramide(float base = 4.0f, float altura = 4.0f, int id = 3);
void desenha_esfera(float raio = 2.0f, int fatias = 30, int stacks = 30, int id = 3);
void desenha_cilindro(float raio = 2.0f, float altura = 4.0f, int fatias = 30, int stacks = 30, bool tampas = true, int id = 3);
void desenha_cone(float raio = 2.0f, float altura = 4.0f, int fatias = 30, int id = 3);
void desenha_torus(float R = 3.0f, float r = 1.0f, int fatias = 30, int stacks = 30, int id = 3);
void desenha_superficie(int formato);
void marcax(float x, float y, float z, float cy, float cp);

/*const int NI = 10, NJ = 10;
const int RESOLUTIONI = 3*NI, RESOLUTIONJ = 3*NJ;
XYZ inpu[NI+1][NJ+1];
XYZ outpu[RESOLUTIONI][RESOLUTIONJ];

double BezierBlend(int k,double mu, int n);
void entrada_inps(int forma, int i, int j);
void generateControlPoint(int forma);*/
//XYZ calculaNormal(XYZ u, XYZ v);
//void Surface(int forma);

#endif