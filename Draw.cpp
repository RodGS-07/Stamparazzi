// Código para Desenhos
#include "Draw.h"
#include "Linear.h"
#include <GL/glut.h>
#include <cmath>

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

void muda_cor(int c){
    glColor3f(cores[c][0],cores[c][1],cores[c][2]);
}

void desenha_chao() {
    glBegin(GL_QUADS);

    // Frente (vermelha)
    muda_cor(0);
    glVertex3f(-1, -1,  1);
    glVertex3f( 1, -1,  1);
    glVertex3f( 1,  1,  1);
    glVertex3f(-1,  1,  1);

    // Trás (verde)
    muda_cor(4);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1,  1, -1);
    glVertex3f( 1,  1, -1);
    glVertex3f( 1, -1, -1);

    // Esquerda (azul)
    muda_cor(6);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1,  1);
    glVertex3f(-1,  1,  1);
    glVertex3f(-1,  1, -1);

    // Direita (amarelo)
    muda_cor(2);
    glVertex3f(1, -1, -1);
    glVertex3f(1,  1, -1);
    glVertex3f(1,  1,  1);
    glVertex3f(1, -1,  1);

    // Topo (ciano)
    muda_cor(5);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1,  1);
    glVertex3f( 1, 1,  1);
    glVertex3f( 1, 1, -1);

    // Base (magenta)
    muda_cor(8);
    glVertex3f(-1, -1, -1);
    glVertex3f( 1, -1, -1);
    glVertex3f( 1, -1,  1);
    glVertex3f(-1, -1,  1);

    glEnd();
}

void desenha_cubo(float lado = 2.0f) {
    glBegin(GL_QUADS);

    glVertex3f(-lado, -lado,  lado);
    glVertex3f( lado, -lado,  lado);
    glVertex3f( lado,  lado,  lado);
    glVertex3f(-lado,  lado,  lado);

    glVertex3f(-lado, -lado, -lado);
    glVertex3f(-lado,  lado, -lado);
    glVertex3f( lado,  lado, -lado);
    glVertex3f( lado, -lado, -lado);

    glVertex3f(-lado, -lado, -lado);
    glVertex3f(-lado, -lado,  lado);
    glVertex3f(-lado,  lado,  lado);
    glVertex3f(-lado,  lado, -lado);

    glVertex3f(lado, -lado, -lado);
    glVertex3f(lado,  lado, -lado);
    glVertex3f(lado,  lado,  lado);
    glVertex3f(lado, -lado,  lado);

    glVertex3f(-lado, lado, -lado);
    glVertex3f(-lado, lado,  lado);
    glVertex3f( lado, lado,  lado);
    glVertex3f( lado, lado, -lado);

    glVertex3f(-lado, -lado, -lado);
    glVertex3f( lado, -lado, -lado);
    glVertex3f( lado, -lado,  lado);
    glVertex3f(-lado, -lado,  lado);

    glEnd();
}

void desenha_piramide(float base = 4.0f, float altura = 4.0f){
    float h = altura;
    float b = base / 2.0f; // metade do tamanho da base

    // --- Base (quadrado no plano y=0) ---
    glBegin(GL_QUADS);
        glVertex3f(-b, -b, -b);
        glVertex3f( b, -b, -b);
        glVertex3f( b, -b,  b);
        glVertex3f(-b, -b,  b);
    glEnd();

    // --- Faces laterais (4 triângulos) ---
    glBegin(GL_TRIANGLES);
        // Frente
        glVertex3f(-b, -b,  b);
        glVertex3f( b, -b,  b);
        glVertex3f( 0.0f,  h-b , 0.0f);

        // Direita
        glVertex3f( b, -b,  b);
        glVertex3f( b, -b, -b);
        glVertex3f( 0.0f,  h-b , 0.0f);

        // Trás
        glVertex3f( b, -b, -b);
        glVertex3f(-b, -b, -b);
        glVertex3f( 0.0f,  h-b , 0.0f);

        // Esquerda
        glVertex3f(-b, -b, -b);
        glVertex3f(-b, -b,  b);
        glVertex3f( 0.0f,  h-b , 0.0f);
    glEnd();
}

void desenha_esfera(float raio = 2.0f, int fatias = 30, int stacks = 30){
    for (int i = 0; i < stacks; ++i) {
        float phi1 = M_PI / 2 - i * (M_PI / stacks);
        float phi2 = M_PI / 2 - (i + 1) * (M_PI / stacks);

        glBegin(GL_QUADS);
        for (int j = 0; j < fatias; ++j) {
            float theta1 = j * (2 * M_PI / fatias);
            float theta2 = (j + 1) * (2 * M_PI / fatias);

            // Vertex 1 (bottom-left of current quad)
            float x1 = raio * cos(phi2) * sin(theta1);
            float y1 = raio * sin(phi2);
            float z1 = raio * cos(phi2) * cos(theta1);
            glVertex3f(x1, y1, z1);

            // Vertex 2 (bottom-right of current quad)
            float x2 = raio * cos(phi2) * sin(theta2);
            float y2 = raio * sin(phi2);
            float z2 = raio * cos(phi2) * cos(theta2);
            glVertex3f(x2, y2, z2);

            // Vertex 3 (top-right of current quad)
            float x3 = raio * cos(phi1) * sin(theta2);
            float y3 = raio * sin(phi1);
            float z3 = raio * cos(phi1) * cos(theta2);
            glVertex3f(x3, y3, z3);

            // Vertex 4 (top-left of current quad)
            float x4 = raio * cos(phi1) * sin(theta1);
            float y4 = raio * sin(phi1);
            float z4 = raio * cos(phi1) * cos(theta1);
            glVertex3f(x4, y4, z4);
        }
        glEnd();
    }
}

void desenha_cilindro(float raio = 2.0f, float altura = 4.0f, int fatias = 30, int stacks = 30, bool tampas = true){
    float half = altura / 2.0f;

    // Superfície lateral
    for (int i = 0; i < stacks; ++i) {
        float z1 = -half + i * (altura / stacks);
        float z2 = -half + (i + 1) * (altura / stacks);

        glBegin(GL_QUADS);
        for (int j = 0; j < fatias; ++j) {
            float theta1 = j * (2 * M_PI / fatias);
            float theta2 = (j + 1) * (2 * M_PI / fatias);

            float x1 = raio * cos(theta1);
            float y1 = raio * sin(theta1);
            float x2 = raio * cos(theta2);
            float y2 = raio * sin(theta2);

            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x1, y1, z2);
        }
        glEnd();
    }

    if(tampas){
        // Tampa inferior
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, -half);
        for (int j = 0; j <= fatias; ++j) {
            float theta = j * (2 * M_PI / fatias);
            float x = raio * cos(theta);
            float y = raio * sin(theta);
            glVertex3f(x, y, -half);
        }
        glEnd();

        // Tampa superior
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, half);
        for (int j = 0; j <= fatias; ++j) {
            float theta = j * (2 * M_PI / fatias);
            float x = raio * cos(theta);
            float y = raio * sin(theta);
            glVertex3f(x, y, half);
        }
        glEnd();
    }
}

void desenha_cone(float raio = 2.0f, float altura = 4.0f, int fatias = 30){
    float half = altura / 2.0f;

    // Superfície lateral
    glBegin(GL_TRIANGLES);
    for (int j = 0; j < fatias; ++j) {
        float theta1 = j * (2 * M_PI / fatias);
        float theta2 = (j + 1) * (2 * M_PI / fatias);

        float x1 = raio * cos(theta1);
        float y1 = raio * sin(theta1);
        float x2 = raio * cos(theta2);
        float y2 = raio * sin(theta2);

        // Triângulo da lateral (base -> ápice)
        glVertex3f(0, 0, half);       // ápice
        glVertex3f(x1, y1, -half);    // base ponto 1
        glVertex3f(x2, y2, -half);    // base ponto 2
    }
    glEnd();

    // Base
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, -half);
    for (int j = 0; j <= fatias; ++j) {
        float theta = j * (2 * M_PI / fatias);
        float x = raio * cos(theta);
        float y = raio * sin(theta);
        glVertex3f(x, y, -half);
    }
    glEnd();
}

void desenha_torus(float R = 3.0f, float r = 1.0f, int fatias = 30, int stacks = 30){
    for (int i = 0; i < stacks; ++i) {
        float phi1 = i * (2 * M_PI / stacks);
        float phi2 = (i + 1) * (2 * M_PI / stacks);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= fatias; ++j) {
            float theta = j * (2 * M_PI / fatias);

            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            float x1 = (R + r * cosTheta) * cos(phi1);
            float y1 = (R + r * cosTheta) * sin(phi1);
            float z1 = r * sinTheta;

            float x2 = (R + r * cosTheta) * cos(phi2);
            float y2 = (R + r * cosTheta) * sin(phi2);
            float z2 = r * sinTheta;

            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }
}

void desenha_superficie(int formato){
    switch(formato){
        case CUBO:
            desenha_cubo();
            break;
        case PIRAMIDE:
            desenha_piramide();
            break;
        case ESFERA:
            desenha_esfera();
            break;
        case CILINDRO:
            desenha_cilindro();
            break;
        case CONE:
            desenha_cone();
            break;
        case TORUS:
            desenha_torus();
            break;
    }
}

void marcax(float x, float y, float z){
    muda_cor(0);
    glLineWidth(5.0f);
    glBegin(GL_LINES);
    glVertex3f(x-2.0f,y-2.0f,z-2.0f);
    glVertex3f(x+2.0f,y+2.0f,z-2.0f);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(x-2.0f,y+2.0f,z-2.0f);
    glVertex3f(x+2.0f,y-2.0f,z-2.0f);
    glEnd();
    glLineWidth(1.0f);
}

const int NI = 10, NJ = 10;
const int RESOLUTIONI = 3*NI, RESOLUTIONJ = 3*NJ;
XYZ inp[NI+1][NJ+1];
XYZ outp[RESOLUTIONI][RESOLUTIONJ];

double BezierBlend(int k,double mu, int n) {
    int nn,kn,nkn;
    double blend=1;
    nn = n;
    kn = k;
    nkn = n - k;

    while (nn >= 1) {
        blend *= nn;
        nn--;
        if (kn > 1) {
            blend /= (double)kn;
            kn--;
        }
        if (nkn > 1) {
            blend /= (double)nkn;
            nkn--;
        }
    }
    if (k > 0)
        blend *= pow(mu,(double)k);
    if (n-k > 0)
        blend *= pow(1-mu,(double)(n-k));
    return(blend);
}

void entrada_inps(int forma, int i, int j){
    // Normaliza parâmetros u, v em [0,1]
    double u = (double)i / (double)NI;  
    double v = (double)j / (double)NJ;  

    // Ângulos padrão
    double theta = 2.0 * M_PI * u;  // giro em torno do eixo
    double phi   = M_PI * v;        // usado em esfera/torus
    float r, R, Rmax, H;

    switch(forma){
        case ESFERA: 
            R = 2.0f; // raio da esfera
            inp[i][j].x = R * sin(phi) * cos(theta);
            inp[i][j].y = R * sin(phi) * sin(theta);
            inp[i][j].z = R * cos(phi);
            break;
        
        case CILINDRO: 
            R = 2.0f; // raio
            H = 4.0f; // altura
            inp[i][j].x = R * cos(theta);
            inp[i][j].y = R * sin(theta);
            inp[i][j].z = (v - 0.5f) * H;
            break;
        
        case CONE: 
            H = 4.0f;       // altura
            Rmax = 2.0f;    // raio da base
            R = (1.0f - v) * Rmax;
            inp[i][j].x = R * cos(theta);
            inp[i][j].y = R * sin(theta);
            inp[i][j].z = v * H;
            break;
        
        case TORUS: 
            R = 3.0f; // raio maior (centro até tubo)
            r = 1.0f; // raio menor (espessura do tubo)
            inp[i][j].x = (R + r * cos(phi)) * cos(theta);
            inp[i][j].y = (R + r * cos(phi)) * sin(theta);
            inp[i][j].z = r * sin(phi);
            break;
        
        case BEZIER: 
            // Exemplo simples: grid de pontos com ondulação
            inp[i][j].x = i;
            inp[i][j].y = j;
            inp[i][j].z = 2.0f * sin(i * 0.5f) * cos(j * 0.5f);
            break;
        
    }
}

void generateControlPoint(int forma) {
    int nMax = 2;
    int nMin = -2;
    int i,j;
    if(forma != CUBO and forma!= PIRAMIDE){
        for (i=0;i<=NI;i++) {
            for (j=0;j<=NJ;j++) {
                entrada_inps(forma,i,j);
            }
        }
    }
}


//cálculos necessários para configurar a normal dos vértices da superfície
XYZ calculaNormal(XYZ u, XYZ v) {
    XYZ normal;
    normal.x = u.y * v.z - u.z * v.y;
    normal.y = u.z * v.x - u.x * v.z;
    normal.z = u.x * v.y - u.y * v.x;


    // Normaliza
    float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    normal.x /= length;
    normal.y /= length;
    normal.z /= length;

    return normal;
}


void Surface(int forma) {
    if(forma != CUBO and forma != PIRAMIDE){
        int i,j,ki,kj;
        double mui,muj,bi,bj;
        for (i=0;i<RESOLUTIONI;i++) {
            mui = i / (double)(RESOLUTIONI-1);
            for (j=0;j<RESOLUTIONJ;j++) {
                muj = j / (double)(RESOLUTIONJ-1);
                outp[i][j].x = 0;
                outp[i][j].y = 0;
                outp[i][j].z = 0;
                for (ki=0;ki<=NI;ki++) {
                    bi = BezierBlend(ki,mui,NI);
                    for (kj=0;kj<=NJ;kj++) {
                    bj = BezierBlend(kj,muj,NJ);
                    outp[i][j].x += (inp[ki][kj].x * bi * bj);
                    outp[i][j].y += (inp[ki][kj].y * bi * bj);
                    outp[i][j].z += (inp[ki][kj].z * bi * bj);
                    }
                }
            }
        }
        for(i=0;i<RESOLUTIONI-1;i++){
            for(j=0;j<RESOLUTIONJ-1;j++){
                XYZ u = {outp[i+1][j].x - outp[i][j].x, outp[i+1][j].y - outp[i][j].y, outp[i+1][j].z - outp[i][j].z};
                XYZ v = {outp[i+1][j+1].x - outp[i][j].x, outp[i+1][j+1].y - outp[i][j].y, outp[i+1][j+1].z - outp[i][j].z};
                XYZ normal = calculaNormal(u,v);
                glBegin(GL_QUADS);
                    glNormal3f(normal.x,normal.y,normal.z); glVertex3f(outp[i][j].x,outp[i][j].y,outp[i][j].z);
                    glNormal3f(normal.x,normal.y,normal.z); glVertex3f(outp[i+1][j].x,outp[i+1][j].y,outp[i+1][j].z);
                    glNormal3f(normal.x,normal.y,normal.z); glVertex3f(outp[i+1][j+1].x,outp[i+1][j+1].y,outp[i+1][j+1].z);
                    glNormal3f(normal.x,normal.y,normal.z); glVertex3f(outp[i][j+1].x,outp[i][j+1].y,outp[i][j+1].z);
                glEnd();
            }
        }
    } else if(forma == CUBO)
        desenha_cubo();
    else {
        desenha_piramide();
    }
}