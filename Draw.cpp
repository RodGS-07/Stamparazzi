// Código para Desenhos
#include "Draw.h"
#include "Linear.h"
#include "Textura.h"
#include <GL/glut.h>
#include <GL/glext.h>
#include <cmath>
#include <vector>

using namespace std;

int cor_atual;

void muda_cor(int c){
    glColor3f(cores[c][0],cores[c][1],cores[c][2]);
    cor_atual = c;
}

int get_cor_atual() {return cor_atual;}

Cor get_cor_struct(int c) {return {cores[c][0],cores[c][1],cores[c][2]};}

void desenha_simbolo_coloradd(int id_cor, float size)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[id_cor]); // ID do símbolo coloradd

    glDisable(GL_LIGHTING); // para não escurecer o símbolo

    glDepthMask(GL_FALSE);

    // Pega a matriz atual (modelo-vista)
    GLfloat modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    // Zera a rotação da matriz (mantém apenas a posição)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            modelview[i*4 + j] = (i == j ? 1.0f : 0.0f);
        }
    }

    // Recarrega matriz "sem rotação" → billboard perfeito
    glLoadMatrixf(modelview);

    if(id_cor!=34) glRotatef(-90,0,0,1);

    glBegin(GL_QUADS);

        // textura sem ficar de cabeça para baixo
        glTexCoord2f(0, 1); glVertex3f(-size, -size, 0);
        glTexCoord2f(1, 1); glVertex3f( size, -size, 0);
        glTexCoord2f(1, 0); glVertex3f( size,  size, 0);
        glTexCoord2f(0, 0); glVertex3f(-size,  size, 0);

    glEnd();

    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_TRUE);
}

void desenha_cubo(float lado, int id_adesivo, bool modo_daltonico) {
    XYZ normal;
    int id_cor = get_cor_atual() + 23;

    glBegin(GL_QUADS);

    // Frente
    normal = Normal({-lado,-lado,lado},{lado,-lado,lado},{lado,lado,lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(-lado, -lado,  lado);
    glVertex3f( lado, -lado,  lado);
    glVertex3f( lado,  lado,  lado);
    glVertex3f(-lado,  lado,  lado);

    // Trás
    normal = Normal({-lado,-lado,-lado},{-lado,lado,-lado},{lado,lado,-lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(-lado, -lado, -lado);
    glVertex3f(lado, -lado, -lado);
    glVertex3f(lado,  lado, -lado);
    glVertex3f(-lado, lado, -lado);

    // Esquerda
    normal = Normal({-lado,-lado,-lado},{-lado,-lado,lado},{-lado,lado,lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(-lado, -lado, -lado);
    glVertex3f(-lado,  lado, -lado);
    glVertex3f(-lado,  lado,  lado);
    glVertex3f(-lado, -lado,  lado);

    // Direita
    normal = Normal({lado,-lado,-lado},{lado,lado,-lado},{lado,lado,lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(lado, -lado, -lado);
    glVertex3f(lado,  lado, -lado);
    glVertex3f(lado,  lado,  lado);
    glVertex3f(lado, -lado,  lado);

    // Topo
    normal = Normal({-lado,lado,-lado},{-lado,lado,lado},{lado,lado,lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(-lado, lado, -lado);
    glVertex3f(lado,  lado, -lado);
    glVertex3f(lado,  lado,  lado);
    glVertex3f(-lado, lado,  lado);

    // Base
    normal = Normal({-lado,-lado,-lado},{lado,-lado,-lado},{lado,-lado,lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(-lado, -lado, -lado);
    glVertex3f(lado,  -lado, -lado);
    glVertex3f(lado,  -lado,  lado);
    glVertex3f(-lado, -lado,  lado);

    glEnd();

    float adesivoTamanho = lado * 0.3f; // 30% da face
    float offset = lado + 0.01f;       // ligeiramente à frente da face

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_FALSE);

    //glEnable(GL_TEXTURE_2D);
//glBindTexture(GL_TEXTURE_2D, texID[id_cor]);

    glColor4f(1, 1, 1, 1);

    float simboloTamanho = lado * 0.85f; // cobre ~85% da face
    offset = lado + 0.002f;        // ligeiramente à frente da superfície

    // if(modo_daltonico) {
        
    //     glPushMatrix();
    //     glTranslatef(0, lado/2 + 2.0f, 0); // cima do polígono
    //     if(id_cor != 34) glRotatef(-90, 0, 0, 1);
    //     desenha_simbolo_coloradd(id_cor);
    //     glPopMatrix();
        
    //     //desenha_simbolo_coloradd(id_cor);
    // }

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);

    //if(modo_daltonico) {

        // // --- Frente ---
        // glBegin(GL_QUADS);
        //     glTexCoord2f(0, 0); glVertex3f(-simboloTamanho, -simboloTamanho, offset);
        //     glTexCoord2f(1, 0); glVertex3f( simboloTamanho, -simboloTamanho, offset);
        //     glTexCoord2f(1, 1); glVertex3f( simboloTamanho,  simboloTamanho, offset);
        //     glTexCoord2f(0, 1); glVertex3f(-simboloTamanho,  simboloTamanho, offset);
        // glEnd();

        // // --- Trás (rotacionada 180° para alinhar) ---
        // glPushMatrix();
        //     glRotatef(90, 0, 0, 1);
        //     glBegin(GL_QUADS);
        //         glTexCoord2f(1, 1); glVertex3f(-simboloTamanho, -simboloTamanho, -offset);
        //         glTexCoord2f(0, 1); glVertex3f( simboloTamanho, -simboloTamanho, -offset);
        //         glTexCoord2f(0, 0); glVertex3f( simboloTamanho,  simboloTamanho, -offset);
        //         glTexCoord2f(1, 0); glVertex3f(-simboloTamanho,  simboloTamanho, -offset);
        //     glEnd();
        // glPopMatrix();

        // // --- Esquerda (rotacionada 90° CW) ---
        // glPushMatrix();
        //     glRotatef(90, 1, 0, 0);
        //     glBegin(GL_QUADS);
        //         glTexCoord2f(0, 1); glVertex3f(-offset, -simboloTamanho, -simboloTamanho);
        //         glTexCoord2f(1, 1); glVertex3f(-offset, -simboloTamanho,  simboloTamanho);
        //         glTexCoord2f(1, 0); glVertex3f(-offset,  simboloTamanho,  simboloTamanho);
        //         glTexCoord2f(0, 0); glVertex3f(-offset,  simboloTamanho, -simboloTamanho);
        //     glEnd();
        // glPopMatrix();

        // // --- Direita (rotacionada 90° CCW) ---
        // glBegin(GL_QUADS);
        //     glTexCoord2f(1, 0); glVertex3f(offset, -simboloTamanho, -simboloTamanho);
        //     glTexCoord2f(0, 0); glVertex3f(offset, -simboloTamanho,  simboloTamanho);
        //     glTexCoord2f(0, 1); glVertex3f(offset,  simboloTamanho,  simboloTamanho);
        //     glTexCoord2f(1, 1); glVertex3f(offset,  simboloTamanho, -simboloTamanho);
        // glEnd();

        // // --- Topo (rotacionada 90° CW) ---
        // glBegin(GL_QUADS);
        //     glTexCoord2f(0, 1); glVertex3f(-simboloTamanho, offset, -simboloTamanho);
        //     glTexCoord2f(1, 1); glVertex3f( simboloTamanho, offset, -simboloTamanho);
        //     glTexCoord2f(1, 0); glVertex3f( simboloTamanho, offset,  simboloTamanho);
        //     glTexCoord2f(0, 0); glVertex3f(-simboloTamanho, offset,  simboloTamanho);
        // glEnd();

        // // --- Base (rotacionada 90° CCW) ---
        // glPushMatrix();
        //     glRotatef(-90, 0, 1, 0);
        //     glBegin(GL_QUADS);
        //         glTexCoord2f(1, 0); glVertex3f(-simboloTamanho, -offset, -simboloTamanho);
        //         glTexCoord2f(0, 0); glVertex3f( simboloTamanho, -offset, -simboloTamanho);
        //         glTexCoord2f(0, 1); glVertex3f( simboloTamanho, -offset,  simboloTamanho);
        //         glTexCoord2f(1, 1); glVertex3f(-simboloTamanho, -offset,  simboloTamanho);
        //     glEnd();
        // glPopMatrix();

        // glDisable(GL_TEXTURE_2D);
        // glDisable(GL_POLYGON_OFFSET_FILL);
    //}

    // Divisão entre textura do ColorADD e do adesivo

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[id_adesivo]);

    glColor4f(1, 1, 1, 1); // mantém as cores originais da textura

    offset += 0.001f;

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-adesivoTamanho, -adesivoTamanho, offset);
        glTexCoord2f(1, 0); glVertex3f( adesivoTamanho, -adesivoTamanho, offset);
        glTexCoord2f(1, 1); glVertex3f( adesivoTamanho,  adesivoTamanho, offset);
        glTexCoord2f(0, 1); glVertex3f(-adesivoTamanho,  adesivoTamanho, offset);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void desenha_piramide(float base, float altura, int id_adesivo, bool modo_daltonico) {
    XYZ normal;
    int id_cor = get_cor_atual() + 23;

    float h = altura;
    float b = base / 2.0f; // metade do tamanho da base

    static float acumulador1 = 0.0f, acumulador2 = 0.0f;

    // --- Base (quadrado no plano y = -b) ---
    normal = Normal({-b,-b,-b},{b,-b,-b},{b,-b,b});
    glNormal3f(normal.x, normal.y, normal.z);
    glBegin(GL_QUADS);
        glVertex3f(-b, -b, -b);
        glVertex3f( b, -b, -b);
        glVertex3f( b, -b,  b);
        glVertex3f(-b, -b,  b);
    glEnd();

    // --- Faces laterais (4 triângulos) ---
    glBegin(GL_TRIANGLES);
        // Frente
        normal = Normal({-b,-b,b},{b,-b,b},{0,h-b,0});
        glNormal3f(normal.x,normal.y,normal.z);
        glVertex3f(-b, -b,  b);
        glVertex3f( b, -b,  b);
        glVertex3f( 0.0f,  h-b , 0.0f);

        // Direita
        normal = Normal({b,-b,b},{b,-b,-b},{0,h-b,0});
        glNormal3f(normal.x,normal.y,normal.z);
        glVertex3f( b, -b,  b);
        glVertex3f( b, -b, -b);
        glVertex3f( 0.0f,  h-b , 0.0f);

        // Trás
        normal = Normal({b,-b,-b},{-b,-b,-b},{0,h-b,0});
        glNormal3f(normal.x,normal.y,normal.z);
        glVertex3f( b, -b, -b);
        glVertex3f(-b, -b, -b);
        glVertex3f( 0.0f,  h-b , 0.0f);

        // Esquerda
        normal = Normal({-b,-b,-b},{-b,-b,b},{0,h-b,0});
        glNormal3f(normal.x,normal.y,normal.z);
        glVertex3f(-b, -b, -b);
        glVertex3f(-b, -b,  b);
        glVertex3f( 0.0f,  h-b , 0.0f);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_FALSE);

    //glEnable(GL_TEXTURE_2D);
//glBindTexture(GL_TEXTURE_2D, texID[id_cor]);

    // if(modo_daltonico) {
        
    //     glPushMatrix();
    //     glTranslatef(0, altura/2 + 1.0f, 0); // cima do polígono
    //     if(id_cor != 34) glRotatef(-90, 0, 0, 1);
    //     desenha_simbolo_coloradd(id_cor);
    //     glPopMatrix();
        
    //     //desenha_simbolo_coloradd(id_cor);
    // }

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);

    // if(modo_daltonico) {
    //     // ---------------------------------------------------------
    //     // Símbolo ColorADD - DESENHADO PRIMEIRO (atrás)
    //     // ---------------------------------------------------------
    //     glEnable(GL_BLEND);
    //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //     glEnable(GL_TEXTURE_2D);
    //     glBindTexture(GL_TEXTURE_2D, texID[id_cor]);
    //     glColor4f(1, 1, 1, 1);

    //     glEnable(GL_POLYGON_OFFSET_FILL);
    //     glPolygonOffset(-1.0f, -1.0f);

    //     float offset_symbol = 0.001f; // levemente acima da superfície
    //     float qsize = b * 0.6f;       // tamanho do quadrado do símbolo

    //     // --- Frente ---
    //     glBegin(GL_QUADS);
    //         glTexCoord2f(0, 0); glVertex3f(-(b/2) * 0.6f, (-b + offset_symbol) * 0.25f,  b * 0.65f);
    //         glTexCoord2f(1, 0); glVertex3f( (b/2) * 0.6f, (-b + offset_symbol) * 0.25f,  b * 0.65f);
    //         glTexCoord2f(1, 1); glVertex3f( (b/2) * 0.4f,  (h * 0.4f) * 0.20f,  b * 0.42f);
    //         glTexCoord2f(0, 1); glVertex3f(-(b/2) * 0.4f,  (h * 0.4f) * 0.20f,  b * 0.42f);
    //     glEnd();

    //     // --- Trás ---
    //     // centro aproximado da área onde o símbolo deve ficar
    //     float centerX = 0.0f;
    //     float centerY = ((-b + offset_symbol) * 0.25f + (h * 0.4f) * 0.20f) * 0.5f;
    //     float centerZ = -b * 0.535f;

    //     float sW = (b/2) * 0.4f; // meio-largura do quad (x local positivo)
    //     float sH = (h * 0.4f) * 0.20f - ((-b + offset_symbol) * 0.25f); // altura local aproximada
    //     if (sH <= 0.0f) sH = qsize * 0.5f; // fallback

    //     glPushMatrix();
    //         // move para o centro da face traseira
    //         glTranslatef(centerX, centerY, centerZ + 0.01f);

    //         // gira para que o quad olhe para -Z (face traseira)
    //         glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

    //         // ajuste para manter o símbolo "em pé" - altere se necessário
    //         glRotatef(0.0f, 0.0f, 0.0f, 1.0f); // geralmente 0 funciona; use 180 para inverter, 90/-90 se estiver rotacionado

    //         glRotatef(-25.0f, 1.0f, 0.0f, 0.0f);

    //         // desenha o quad no plano local (z = 0)
    //         float sx = (b/2) * 0.6f * 0.5f; // metade da largura local do símbolo
    //         float sy = ( (h * 0.4f) * 0.20f - ((-b + offset_symbol) * 0.25f) ) * 0.5f;
    //         if (sy <= 0.0f) sy = qsize * 0.5f;

    //         glBegin(GL_QUADS);
    //             glTexCoord2f(0.0f, 0.0f); glVertex3f(-sx, -sy, 0.0f);
    //             glTexCoord2f(1.0f, 0.0f); glVertex3f( sx, -sy, 0.0f);
    //             glTexCoord2f(1.0f, 1.0f); glVertex3f( sx,  sy, 0.0f);
    //             glTexCoord2f(0.0f, 1.0f); glVertex3f(-sx,  sy, 0.0f);
    //         glEnd();
    //     glPopMatrix();
    //     // glBegin(GL_QUADS);
    //     //     glTexCoord2f(0, 0); glVertex3f(-(b/2) * 0.6f, (-b + offset_symbol) * 0.25f, -b * 0.65f);
    //     //     glTexCoord2f(1, 0); glVertex3f( (b/2) * 0.6f, (-b + offset_symbol) * 0.25f, -b * 0.65f);
    //     //     glTexCoord2f(1, 1); glVertex3f( (b/2) * 0.4f,  (h * 0.4f) * 0.20f, -b * 0.42f);
    //     //     glTexCoord2f(0, 1); glVertex3f(-(b/2) * 0.4f,  (h * 0.4f) * 0.20f, -b * 0.42f);
    //     // glEnd();

    //     // --- Direita ---
    //     glBegin(GL_QUADS);
    //         glTexCoord2f(0, 0); glVertex3f( b * 0.65f, (-b + offset_symbol) * 0.25f,  (b/2) * 0.6f);
    //         glTexCoord2f(1, 0); glVertex3f( b * 0.65f, (-b + offset_symbol) * 0.25f, -(b/2) * 0.6f);
    //         glTexCoord2f(1, 1); glVertex3f( b * 0.42f,  (h * 0.4f) * 0.20f, -(b/2) * 0.4f);
    //         glTexCoord2f(0, 1); glVertex3f( b * 0.42f,  (h * 0.4f) * 0.20f,  (b/2) * 0.4f);
    //     glEnd();

    //     // --- Esquerda ---
    //     // Posição central aproximada do símbolo
    //     static float acum_x = 0.0f;

    //     centerX = -b * 0.535f;
    //     centerY = ((-b + offset_symbol) * 0.25f + (h * 0.4f) * 0.20f) * 0.5f;
    //     centerZ = 0.0f;

    //     // Dimensões locais do símbolo
    //     sx = (b/2) * 0.6f * 0.5f;
    //     sy = ((h * 0.4f) * 0.20f - ((-b + offset_symbol) * 0.25f)) * 0.5f;
    //     if (sy <= 0.0f) sy = qsize * 0.5f;

    //     glPushMatrix();
    //         // move até o centro da face esquerda
    //         glTranslatef(centerX - 0.01f, centerY, centerZ);

    //         glRotatef(-25.0f, 0.0f, 0.0f, 1.0f);
            
    //         glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

    //         // desenha o símbolo plano
    //         glBegin(GL_QUADS);
    //             glTexCoord2f(0.0f, 0.0f); glVertex3f(-sx, -sy, 0.0f);
    //             glTexCoord2f(1.0f, 0.0f); glVertex3f( sx, -sy, 0.0f);
    //             glTexCoord2f(1.0f, 1.0f); glVertex3f( sx,  sy, 0.0f);
    //             glTexCoord2f(0.0f, 1.0f); glVertex3f(-sx,  sy, 0.0f);
    //         glEnd();
    //     glPopMatrix();
    //     // glBegin(GL_QUADS);
    //     //     glTexCoord2f(0, 0); glVertex3f(-b * 0.65f, (-b + offset_symbol) * 0.25f,  (b/2) * 0.6f);
    //     //     glTexCoord2f(1, 0); glVertex3f(-b * 0.65f, (-b + offset_symbol) * 0.25f, -(b/2) * 0.6f);
    //     //     glTexCoord2f(1, 1); glVertex3f(-b * 0.42f,  (h * 0.4f) * 0.20f, -(b/2) * 0.4f);
    //     //     glTexCoord2f(0, 1); glVertex3f(-b * 0.42f,  (h * 0.4f) * 0.20f,  (b/2) * 0.4f);
    //     // glEnd();

    //     // --- Base ---
    //     glBegin(GL_QUADS);
    //         glTexCoord2f(0, 0); glVertex3f(-b*0.5f, -b - offset_symbol, -b*0.5f);
    //         glTexCoord2f(1, 0); glVertex3f( b*0.5f, -b - offset_symbol, -b*0.5f);
    //         glTexCoord2f(1, 1); glVertex3f( b*0.5f, -b - offset_symbol,  b*0.5f);
    //         glTexCoord2f(0, 1); glVertex3f(-b*0.5f, -b - offset_symbol,  b*0.5f);
    //     glEnd();

    //     glDisable(GL_TEXTURE_2D);
    //     glDisable(GL_POLYGON_OFFSET_FILL);
    // }

    // ---------------------------------------------------------
    // Adesivo - DESENHADO DEPOIS (na frente)
    // ---------------------------------------------------------
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    float offset_adesivo = 0.003f; // maior que o do símbolo → fica na frente
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[id_adesivo]);
    glColor4f(1, 1, 1, 1);

    glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 0); glVertex3f(-(b/2) * 0.5f, (-b + offset_adesivo) * 0.25f,  b*0.65f);
        glTexCoord2f(1, 0); glVertex3f( (b/2) * 0.5f, (-b + offset_adesivo) * 0.25f,  b*0.65f);
        glTexCoord2f(0.5, 1); glVertex3f( 0,  (h*0.4f) * 0.15f,  b*0.45f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void desenha_esfera(float raio, int fatias, int stacks, int id_adesivo, bool modo_daltonico){
    int id_cor = get_cor_atual() + 23;

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

            // Vertex 2 (bottom-right of current quad)
            float x2 = raio * cos(phi2) * sin(theta2);
            float y2 = raio * sin(phi2);
            float z2 = raio * cos(phi2) * cos(theta2);

            // Vertex 3 (top-right of current quad)
            float x3 = raio * cos(phi1) * sin(theta2);
            float y3 = raio * sin(phi1);
            float z3 = raio * cos(phi1) * cos(theta2);

            // Vertex 4 (top-left of current quad)
            float x4 = raio * cos(phi1) * sin(theta1);
            float y4 = raio * sin(phi1);
            float z4 = raio * cos(phi1) * cos(theta1);

            //XYZ normal = Normal({x1,y1,z1},{x2,y2,z2},{x3,y3,z3});
            //glNormal3f(normal.x,normal.y,normal.z);
            glNormal3f(x1/raio,y1/raio,z1/raio); glVertex3f(x1, y1, z1);
            glNormal3f(x2/raio,y2/raio,z2/raio); glVertex3f(x2, y2, z2);
            glNormal3f(x3/raio,y3/raio,z3/raio); glVertex3f(x3, y3, z3);
            glNormal3f(x4/raio,y4/raio,z4/raio); glVertex3f(x4, y4, z4);
        }
        glEnd();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_FALSE);

    //glEnable(GL_TEXTURE_2D);
//glBindTexture(GL_TEXTURE_2D, texID[id_cor]);

    // if(modo_daltonico) {
        
    //     glPushMatrix();
    //     glTranslatef(0, raio + 1.0f, 0); // cima do polígono
    //     if(id_cor != 34) glRotatef(-90, 0, 0, 1);
    //     desenha_simbolo_coloradd(id_cor);
    //     glPopMatrix();
        
    //     //desenha_simbolo_coloradd(id_cor);
    // }

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);

    // if(modo_daltonico) {
    //     // --- Símbolo ColorADD nas laterais ---
    //     glEnable(GL_POLYGON_OFFSET_FILL);
    //     glPolygonOffset(-1.0f, -1.0f);

    //     glEnable(GL_BLEND);
    //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //     glEnable(GL_TEXTURE_2D);
    //     glBindTexture(GL_TEXTURE_2D, texID[id_cor]);
    //     glColor4f(1, 1, 1, 1);

    //     float simboloRaio = raio * 1.0005f;
    //     float simboloAltura = M_PI / 6;   // ~30° de altura (zona equatorial)
    //     float simboloLargura = M_PI / 2;  // 90° em torno do equador
    //     int subdiv = 40;

    //     // --- Frente ---
    //     for (int i = 0; i < subdiv; ++i) {
    //         float phi1 = -simboloAltura / 2 + i * (simboloAltura / subdiv);
    //         float phi2 = -simboloAltura / 2 + (i + 1) * (simboloAltura / subdiv);

    //         glBegin(GL_QUAD_STRIP);
    //         for (int j = 0; j <= subdiv; ++j) {
    //             float theta = -simboloLargura / 2 + j * (simboloLargura / subdiv);

    //             // Faixa no equador (em torno de Y = 0)
    //             float x1 = simboloRaio * cos(phi2) * sin(theta);
    //             float y1 = simboloRaio * sin(phi2);
    //             float z1 = simboloRaio * cos(phi2) * cos(theta);

    //             float x2 = simboloRaio * cos(phi1) * sin(theta);
    //             float y2 = simboloRaio * sin(phi1);
    //             float z2 = simboloRaio * cos(phi1) * cos(theta);

    //             float u = (theta + simboloLargura / 2) / simboloLargura;
    //             float v1 = (phi2 + simboloAltura / 2) / simboloAltura;
    //             float v2 = (phi1 + simboloAltura / 2) / simboloAltura;

    //             glTexCoord2f(u, v1); glVertex3f(x1, y1, z1);
    //             glTexCoord2f(u, v2); glVertex3f(x2, y2, z2);
    //         }
    //         glEnd();
    //     }

    //     // --- Lateral esquerda ---
    //     for (int i = 0; i < subdiv; ++i) {
    //         float phi1 = -simboloAltura / 2 + i * (simboloAltura / subdiv);
    //         float phi2 = -simboloAltura / 2 + (i + 1) * (simboloAltura / subdiv);

    //         glBegin(GL_QUAD_STRIP);
    //         for (int j = 0; j <= subdiv; ++j) {
    //             float theta = -M_PI / 2 - simboloLargura / 2 + j * (simboloLargura / subdiv);

    //             float x1 = simboloRaio * cos(phi2) * sin(theta);
    //             float y1 = simboloRaio * sin(phi2);
    //             float z1 = simboloRaio * cos(phi2) * cos(theta);

    //             float x2 = simboloRaio * cos(phi1) * sin(theta);
    //             float y2 = simboloRaio * sin(phi1);
    //             float z2 = simboloRaio * cos(phi1) * cos(theta);

    //             float u = (theta + simboloLargura / 2) / simboloLargura;
    //             float v1 = (phi2 + simboloAltura / 2) / simboloAltura;
    //             float v2 = (phi1 + simboloAltura / 2) / simboloAltura;

    //             glTexCoord2f(u, v1); glVertex3f(x1, y1, z1);
    //             glTexCoord2f(u, v2); glVertex3f(x2, y2, z2);
    //         }
    //         glEnd();
    //     }

    //     // --- Lateral direita ---
    //     for (int i = 0; i < subdiv; ++i) {
    //         float phi1 = -simboloAltura / 2 + i * (simboloAltura / subdiv);
    //         float phi2 = -simboloAltura / 2 + (i + 1) * (simboloAltura / subdiv);

    //         glBegin(GL_QUAD_STRIP);
    //         for (int j = 0; j <= subdiv; ++j) {
    //             float theta = M_PI / 2 - simboloLargura / 2 + j * (simboloLargura / subdiv);

    //             float x1 = simboloRaio * cos(phi2) * sin(theta);
    //             float y1 = simboloRaio * sin(phi2);
    //             float z1 = simboloRaio * cos(phi2) * cos(theta);

    //             float x2 = simboloRaio * cos(phi1) * sin(theta);
    //             float y2 = simboloRaio * sin(phi1);
    //             float z2 = simboloRaio * cos(phi1) * cos(theta);

    //             float u = (theta + simboloLargura / 2) / simboloLargura;
    //             float v1 = (phi2 + simboloAltura / 2) / simboloAltura;
    //             float v2 = (phi1 + simboloAltura / 2) / simboloAltura;

    //             glTexCoord2f(u, v1); glVertex3f(x1, y1, z1);
    //             glTexCoord2f(u, v2); glVertex3f(x2, y2, z2);
    //         }
    //         glEnd();
    //     }

    //     // --- Traseira ---
    //     for (int i = 0; i < subdiv; ++i) {
    //         float phi1 = -simboloAltura / 2 + i * (simboloAltura / subdiv);
    //         float phi2 = -simboloAltura / 2 + (i + 1) * (simboloAltura / subdiv);

    //         glBegin(GL_QUAD_STRIP);
    //         for (int j = 0; j <= subdiv; ++j) {
    //             float theta = M_PI - simboloLargura / 2 + j * (simboloLargura / subdiv);

    //             float x1 = simboloRaio * cos(phi2) * sin(theta);
    //             float y1 = simboloRaio * sin(phi2);
    //             float z1 = simboloRaio * cos(phi2) * cos(theta);

    //             float x2 = simboloRaio * cos(phi1) * sin(theta);
    //             float y2 = simboloRaio * sin(phi1);
    //             float z2 = simboloRaio * cos(phi1) * cos(theta);

    //             float u = (theta + simboloLargura / 2) / simboloLargura;
    //             float v1 = (phi2 + simboloAltura / 2) / simboloAltura;
    //             float v2 = (phi1 + simboloAltura / 2) / simboloAltura;

    //             glTexCoord2f(u, v1); glVertex3f(x1, y1, z1);
    //             glTexCoord2f(u, v2); glVertex3f(x2, y2, z2);
    //         }
    //         glEnd();
    //     }

    //     glDisable(GL_TEXTURE_2D);
    //     glDisable(GL_POLYGON_OFFSET_FILL);
    // }

    // --- Adesivo na frente da esfera ---

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    float adesivoRaio = raio * 1.001f; // ligeiramente menor (pra evitar z-fighting)
    float adesivoAngulo = M_PI / 6;    // “tamanho” angular do adesivo (~30°)
    float fatiasAdesivo = 20;          // resolução do adesivo

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[id_adesivo]);

    glColor4f(1, 1, 1, 1);

    for (int i = 0; i < fatiasAdesivo; ++i) {
        float phi1 = -adesivoAngulo / 2 + i * (adesivoAngulo / fatiasAdesivo);
        float phi2 = -adesivoAngulo / 2 + (i + 1) * (adesivoAngulo / fatiasAdesivo);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= fatiasAdesivo; ++j) {
            float theta = -adesivoAngulo / 2 + j * (adesivoAngulo / fatiasAdesivo);

            // Ponto da “fileira” superior
            float x1 = adesivoRaio * cos(phi2) * sin(theta);
            float y1 = adesivoRaio * sin(phi2);
            float z1 = adesivoRaio * cos(phi2) * cos(theta);

            // Ponto da “fileira” inferior
            float x2 = adesivoRaio * cos(phi1) * sin(theta);
            float y2 = adesivoRaio * sin(phi1);
            float z2 = adesivoRaio * cos(phi1) * cos(theta);

            // Coordenadas de textura proporcionais
            float u = (theta + adesivoAngulo / 2) / adesivoAngulo;
            float v1 = (phi2 + adesivoAngulo / 2) / adesivoAngulo;
            float v2 = (phi1 + adesivoAngulo / 2) / adesivoAngulo;

            glTexCoord2f(u, v1); glVertex3f(x1, y1, z1);
            glTexCoord2f(u, v2); glVertex3f(x2, y2, z2);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void desenha_cilindro(float raio, float altura, int fatias, int stacks, bool tampas, int id_adesivo, bool modo_daltonico){
    float half = altura / 2.0f;
    XYZ normal;
    int id_cor = get_cor_atual() + 23;

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

            //normal = Normal({x1,y1,z1},{x2,y2,z1},{x2,y2,z2});
            //glNormal3f(normal.x,normal.y,normal.z);
            glNormal3f(x1/raio,y1/raio,0); glVertex3f(x1, y1, z1);
            glNormal3f(x2/raio,y2/raio,0); glVertex3f(x2, y2, z1);
            glNormal3f(x2/raio,y2/raio,0); glVertex3f(x2, y2, z2);
            glNormal3f(x1/raio,y1/raio,0); glVertex3f(x1, y1, z2);
        }
        glEnd();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_FALSE);

    //glEnable(GL_TEXTURE_2D);
//glBindTexture(GL_TEXTURE_2D, texID[id_cor]);

    // if(modo_daltonico) {
        
    //     glPushMatrix();
    //     glTranslatef(0, altura/2 + 1.0f, 0); // cima do polígono
    //     if(id_cor != 34) glRotatef(-90, 0, 0, 1);
    //     desenha_simbolo_coloradd(id_cor);
    //     glPopMatrix();
        
    //     //desenha_simbolo_coloradd(id_cor);
    // }

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);

    if(tampas){
        // Tampa inferior
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, -half);
        for (int j = 0; j <= fatias; ++j) {
            float theta = j * (2 * M_PI / fatias);
            float x = raio * cos(theta);
            float y = raio * sin(theta);
            glNormal3f(0,0,-1);
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
            glNormal3f(0,0,1);
            glVertex3f(x, y, half);
        }
        glEnd();

        // if(modo_daltonico) {
        //     // =======================================================
        //     // SÍMBOLO COLORADD NAS TAMPAS (atrás do adesivo)
        //     // =======================================================
        //     glEnable(GL_BLEND);
        //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //     glEnable(GL_TEXTURE_2D);
        //     glBindTexture(GL_TEXTURE_2D, texID[id_cor]);
        //     glColor4f(1, 1, 1, 1);

        //     glEnable(GL_POLYGON_OFFSET_FILL);
        //     glPolygonOffset(-1.0f, -1.0f);

        //     float simboloRaio = raio * 0.6f;  // maior que o adesivo
        //     float offsetSimbolo = half + 0.008f; // ligeiramente abaixo do adesivo

        //     // --- Tampa superior ---
        //     glBegin(GL_TRIANGLE_FAN);
        //         glNormal3f(0, 0, 1);
        //         glTexCoord2f(0.5f, 0.5f);
        //         glVertex3f(0, 0, offsetSimbolo);
        //         for (int j = 0; j <= fatias; ++j) {
        //             float theta = j * (2 * M_PI / fatias);
        //             float x = simboloRaio * cos(theta);
        //             float y = simboloRaio * sin(theta);
        //             float u = 0.5f + 0.5f * cos(theta);
        //             float v = 0.5f + 0.5f * sin(theta);
        //             glTexCoord2f(u, v);
        //             glVertex3f(x, y, offsetSimbolo);
        //         }
        //     glEnd();

        //     // --- Tampa inferior ---
        //     glBegin(GL_TRIANGLE_FAN);
        //         glNormal3f(0, 0, -1);
        //         glTexCoord2f(0.5f, 0.5f);
        //         glVertex3f(0, 0, -half - 0.008f);
        //         for (int j = 0; j <= fatias; ++j) {
        //             float theta = j * (2 * M_PI / fatias);
        //             float x = simboloRaio * cos(theta);
        //             float y = simboloRaio * sin(theta);
        //             float u = 0.5f + 0.5f * cos(theta);
        //             float v = 0.5f + 0.5f * sin(theta);
        //             glTexCoord2f(u, v);
        //             glVertex3f(x, y, -half - 0.008f);
        //         }
        //     glEnd();

        //     // =======================================================
        //     // SÍMBOLO COLORADD NA LATERAL  (corrigido: sem repeat)
        //     // =======================================================
        //     float simboloAltura = altura * 0.4f;   // ocupa 40% da altura
        //     float simboloAngulo = M_PI / 4;        // cobre 45° da lateral

        //     float z1 = -simboloAltura / 2;
        //     float z2 =  simboloAltura / 2;
        //     float r = raio * 1.01f;

        //     // Antes de bind: força CLAMP para evitar repetição nas bordas
        //     glEnable(GL_TEXTURE_2D);
        //     glBindTexture(GL_TEXTURE_2D, texID[id_cor]);

        //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //     glPushMatrix();
        //         glRotatef(180, 0, 1, 0);
        //         glRotatef(90, 0, 0, 1);
        //         glBegin(GL_QUADS);
        //             int segments = max(1, fatias / 8); // continue usando ~45° de arco
        //             for (int j = 0; j < segments; ++j) { // note: use < segments (não <=)
        //                 float theta1 = -simboloAngulo / 2 + j * (simboloAngulo / segments);
        //                 float theta2 = -simboloAngulo / 2 + (j + 1) * (simboloAngulo / segments);

        //                 float x1 = r * cos(theta1);
        //                 float y1 = r * sin(theta1);
        //                 float x2 = r * cos(theta2);
        //                 float y2 = r * sin(theta2);

        //                 // mapear u estritamente entre 0 e 1 para cada segmento
        //                 float u1 = (float)j / (float)segments;
        //                 float u2 = (float)(j+1) / (float)segments;

        //                 glTexCoord2f(u1, 0); glVertex3f(x1, y1, z1);
        //                 glTexCoord2f(u2, 0); glVertex3f(x2, y2, z1);
        //                 glTexCoord2f(u2, 1); glVertex3f(x2, y2, z2);
        //                 glTexCoord2f(u1, 1); glVertex3f(x1, y1, z2);
        //             }
        //         glEnd();
        //     glPopMatrix();

        //     // (opcional) restaurar wrap para o estado anterior (se quiser)
        //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //     glDisable(GL_TEXTURE_2D);
        //     glDisable(GL_POLYGON_OFFSET_FILL);
        // }

        // --- Adesivo na tampa superior ---
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f);
        
        float adesivoRaio = raio * 0.4f; // menor que o raio da tampa
        float offset = half + 0.01f;    // um pouquinho acima da tampa

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texID[id_adesivo]);

        glColor4f(1, 1, 1, 1); // mantém a cor da textura

        glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0, 0, 1);
            glTexCoord2f(0.5f, 0.5f); // centro da textura
            glVertex3f(0, 0, offset);

            for (int j = 0; j <= fatias; ++j) {
                float theta = j * (2 * M_PI / fatias);
                float x = adesivoRaio * cos(theta);
                float y = adesivoRaio * sin(theta);

                // Mapeamento da textura de forma circular
                float u = 0.5f + 0.5f * cos(theta);
                float v = 0.5f + 0.5f * sin(theta);

                glTexCoord2f(u, v);
                glVertex3f(x, y, offset);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void desenha_cone(float raio, float altura, int fatias, int id_adesivo, bool modo_daltonico){
    float half = altura / 2.0f;
    XYZ normal;
    int id_cor = get_cor_atual() + 23;

    // Superfície lateral
    glBegin(GL_TRIANGLES);
    for (int j = 0; j < fatias; ++j) {
        float theta1 = j * (2 * M_PI / fatias);
        float theta2 = (j + 1) * (2 * M_PI / fatias);

        float x1 = raio * cos(theta1);
        float y1 = raio * sin(theta1);
        float x2 = raio * cos(theta2);
        float y2 = raio * sin(theta2);

        // Normal no ponto 1 da base
        float nx1 = x1;
        float ny1 = y1;
        float nz1 = raio / altura;   // slope
        float len1 = sqrt(nx1*nx1 + ny1*ny1 + nz1*nz1);
        nx1 /= len1; ny1 /= len1; nz1 /= len1;

        // Normal no ponto 2 da base
        float nx2 = x2;
        float ny2 = y2;
        float nz2 = raio / altura;
        float len2 = sqrt(nx2*nx2 + ny2*ny2 + nz2*nz2);
        nx2 /= len2; ny2 /= len2; nz2 /= len2;

        // Normal no ápice: mesma direção média dos lados
        float nxA = (nx1 + nx2) * 0.5f;
        float nyA = (ny1 + ny2) * 0.5f;
        float nzA = (nz1 + nz2) * 0.5f;
        float lenA = sqrt(nxA*nxA + nyA*nyA + nzA*nzA);
        nxA /= lenA; nyA /= lenA; nzA /= lenA;

        // Triângulo da lateral (base -> ápice)
        glNormal3f(nxA,nyA,nzA); glVertex3f(0, 0, half);       // ápice
        glNormal3f(nx1,ny1,nz1); glVertex3f(x1, y1, -half);    // base ponto 1
        glNormal3f(nx2,ny2,nz2); glVertex3f(x2, y2, -half);    // base ponto 2
    }
    glEnd();

    // Base
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, -half);
    for (int j = 0; j <= fatias; ++j) {
        float theta = j * (2 * M_PI / fatias);
        float x = raio * cos(theta);
        float y = raio * sin(theta);
        glNormal3f(0,0,-1);
        glVertex3f(x, y, -half);
    }
    glEnd();

    float adesivoAltura, adesivoLargura, rOffset, zSimbolo, theta, xDir, yDir, adesivoZ, s, h, slopeAngle;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_FALSE);

    //glEnable(GL_TEXTURE_2D);
//glBindTexture(GL_TEXTURE_2D, texID[id_cor]);

    // if(modo_daltonico) {
        
    //     glPushMatrix();
    //     glTranslatef(0, altura/2 + 1.0f, 0); // cima do polígono
    //     if(id_cor != 34) glRotatef(-90, 0, 0, 1);
    //     desenha_simbolo_coloradd(id_cor);
    //     glPopMatrix();
        
    //     //desenha_simbolo_coloradd(id_cor);
    // }

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);

    // if(modo_daltonico) {
    //     // ==========================================================
    //     // SÍMBOLO COLORADD (base e lateral inferior)
    //     // ==========================================================
    //     glEnable(GL_BLEND);
    //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //     glEnable(GL_TEXTURE_2D);
    //     glBindTexture(GL_TEXTURE_2D, texID[id_cor]);
    //     glColor4f(1, 1, 1, 1);

    //     glEnable(GL_POLYGON_OFFSET_FILL);
    //     glPolygonOffset(-1.0f, -1.0f);

    //     // --- Base circular (como tampa inferior) ---
    //     float simboloRaio = raio * 0.75f;
    //     float offsetBase = -half - 0.008f; // um pouco abaixo da base
    //     glPushMatrix();
    //         glRotatef(-90, 0, 0, 1);
    //         glBegin(GL_TRIANGLE_FAN);
    //             glNormal3f(0, 0, -1);
    //             glTexCoord2f(0.5f, 0.5f);
    //             glVertex3f(0, 0, offsetBase);
    //             for (int j = 0; j <= fatias; ++j) {
    //                 float theta = j * (2 * M_PI / fatias);
    //                 float x = simboloRaio * cos(theta);
    //                 float y = simboloRaio * sin(theta);
    //                 float u = 0.5f + 0.5f * cos(theta);
    //                 float v = 0.5f + 0.5f * sin(theta);
    //                 glTexCoord2f(u, v);
    //                 glVertex3f(x, y, offsetBase);
    //             }
    //         glEnd();
    //     glPopMatrix();

    //     // --- Lateral: símbolo abaixo do adesivo ---
    //     adesivoAltura = altura * 0.2f;
    //     adesivoLargura = raio * 0.8f;
    //     rOffset = raio * 0.97f; // um pouco mais dentro que o adesivo
    //     zSimbolo = half - adesivoAltura * 0.8f; //-half + (altura * 0.25f); // abaixo do adesivo
    //     theta = 0.0f; // direção frontal (Z positivo)

    //     xDir = rOffset * sin(theta);
    //     yDir = rOffset * cos(theta);

    //     glPushMatrix();
    //         glTranslatef(xDir, yDir - 1.6f, zSimbolo);

    //         // Mesmo ângulo de inclinação da lateral
    //         slopeAngle = 63.75f;
    //         glRotatef(-slopeAngle, 1, 0, 0);

    //         s = adesivoLargura * 0.6f; // menor que o adesivo
    //         h = adesivoAltura * 0.4f;  // menor também

    //         glBegin(GL_QUADS);
    //             glTexCoord2f(0, 0); glVertex3f(-s, -h, 0);
    //             glTexCoord2f(1, 0); glVertex3f( s, -h, 0);
    //             glTexCoord2f(1, 1); glVertex3f( s,  h, 0);
    //             glTexCoord2f(0, 1); glVertex3f(-s,  h, 0);
    //         glEnd();
    //     glPopMatrix();

    //     glDisable(GL_TEXTURE_2D);
    //     glDisable(GL_POLYGON_OFFSET_FILL);
    // }

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    // Configura textura
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[id_adesivo]);

    glColor4f(1, 1, 1, 1); // mantém cores da textura

    // ---------------- Adesivo na lateral ----------------
    // O adesivo será colado no lado "frontal" do cone (direção +Z)
    adesivoAltura = altura * 0.3f;  // fração da altura
    adesivoLargura = raio * 0.8f;   // fração da largura
    float zOffset = half - adesivoAltura * 0.8f; // mais próximo do ápice
    rOffset = raio * 0.98f;         // quase colado à lateral

    // Calcula ângulo médio da frente (+Z)
    theta = 0.0f; // direção frontal (Z positivo)
    xDir = rOffset * sin(theta);
    yDir = rOffset * cos(theta);

    // Posição média do adesivo na superfície
    adesivoZ = -half + adesivoAltura * 0.5f;

    // Coloca o adesivo orientado tangente à superfície
    glPushMatrix();
        // Move o adesivo até a posição na lateral do cone
        glTranslatef(xDir, yDir - 0.26f, adesivoZ);

        // Calcula ângulo de inclinação da superfície do cone
        slopeAngle = 63.75f; //atan(raio / altura) * 180.0f / M_PI + 36.5f;

        // Rotaciona o adesivo para acompanhar a inclinação do cone
        glRotatef(-slopeAngle, 1, 0, 0);

        // Rotaciona para ficar de frente (+Z)
        //glRotatef(90, 0, 1, 0);

        s = adesivoLargura * 0.5f;
        h = adesivoAltura * 0.5f;

        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f(-s, -h, 0);
            glTexCoord2f(1, 0); glVertex3f( s, -h, 0);
            glTexCoord2f(1, 1); glVertex3f( s,  h, 0);
            glTexCoord2f(0, 1); glVertex3f(-s,  h, 0);
        glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void desenha_torus(float R, float r, int fatias, int stacks, int id_adesivo, bool modo_daltonico){
    vector<vector<float>> mat(3,vector<float> (3));
    int id_cor = get_cor_atual() + 23;

    for (int i = 0; i < stacks; ++i) {
        float phi1 = i * (2 * M_PI / stacks);
        float phi2 = (i + 1) * (2 * M_PI / stacks);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= fatias; ++j) {
            float theta = j * (2 * M_PI / fatias);

            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            // Ponto em phi1
            float x1 = (R + r * cosTheta) * cos(phi1);
            float y1 = (R + r * cosTheta) * sin(phi1);
            float z1 = r * sinTheta;

            // Normal em phi1
            float nx1 = cosTheta * cos(phi1);
            float ny1 = cosTheta * sin(phi1);
            float nz1 = sinTheta;

            // Ponto em phi2
            float x2 = (R + r * cosTheta) * cos(phi2);
            float y2 = (R + r * cosTheta) * sin(phi2);
            float z2 = r * sinTheta;

            // Normal em phi2
            float nx2 = cosTheta * cos(phi2);
            float ny2 = cosTheta * sin(phi2);
            float nz2 = sinTheta;

            // Passa normais + vértices
            glNormal3f(nx1, ny1, nz1);
            glVertex3f(x1, y1, z1);

            glNormal3f(nx2, ny2, nz2);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }

    // ---------- Define posição e lado do adesivo ----------
    bool ladoDireito = 1; // 1 = direito

    float angulo = ladoDireito ? M_PI / 2 : -M_PI / 2; // +90° ou -90°
    float xAdesivo = R * cos(angulo);
    float yAdesivo = R * sin(angulo);
    float zAdesivo = 0.0f;

    float offset, tamanho, s;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_FALSE);

    //glEnable(GL_TEXTURE_2D);
//glBindTexture(GL_TEXTURE_2D, texID[id_cor]);

    // if(modo_daltonico) {
        
    //     glPushMatrix();
    //     glTranslatef(0, R + 2.0f, 0); // cima do polígono
    //     if(id_cor != 34) glRotatef(-90, 0, 0, 1);
    //     desenha_simbolo_coloradd(id_cor);
    //     glPopMatrix();
        
    //     //desenha_simbolo_coloradd(id_cor);
    // }

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);

    // if(modo_daltonico) {
    //     // ==========================================================
    //     // SÍMBOLOS COLORADD NAS LATERAIS
    //     // ==========================================================
    //     glEnable(GL_BLEND);
    //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //     glEnable(GL_TEXTURE_2D);
    //     glBindTexture(GL_TEXTURE_2D, texID[id_cor]);
    //     glColor4f(1, 1, 1, 1);

    //     glEnable(GL_POLYGON_OFFSET_FILL);
    //     glPolygonOffset(-1.0f, -1.0f);

    //     offset = 0.003f;      // deslocamento para fora
    //     tamanho = r * 1.4f;   // tamanho do símbolo
    //     s = tamanho * 0.5f;

    //     // Posições: direita (+X) e esquerda (-X)
    //     float posicoes[2] = {0.0f, M_PI};

    //     for (int i = 0; i < 2; ++i) {
    //         float phi = posicoes[i];
    //         float x = R * cos(phi);
    //         float y = R * sin(phi);

    //         glPushMatrix();
    //             // Move o símbolo para o lado do torus
    //             glTranslatef(x, y, 0.0f);

    //             // Rotaciona para tangenciar a curvatura lateral
    //             if(!i) {
    //                 glRotatef(phi * 180.0f / M_PI, 0, 0, 1);  // acompanha lado (+X / -X)
    //                 glRotatef(90, 0, 1, 0);                   // plano voltado para fora
    //             } else {
    //                 glRotatef(-90, 0, 1, 0);
    //             }

    //             // Desloca levemente para fora da superfície
    //             glTranslatef(0, 0, r + offset);

    //             // Desenha o quadrado do símbolo
    //             glBegin(GL_QUADS);
    //                 glTexCoord2f(0, 0); glVertex3f(-s, -s, 0);
    //                 glTexCoord2f(1, 0); glVertex3f( s, -s, 0);
    //                 glTexCoord2f(1, 1); glVertex3f( s,  s, 0);
    //                 glTexCoord2f(0, 1); glVertex3f(-s,  s, 0);
    //             glEnd();
    //         glPopMatrix();
    //     }

    //     glDisable(GL_TEXTURE_2D);
    //     glDisable(GL_POLYGON_OFFSET_FILL);
    // }

    offset = 0.002f; // evita z-fighting
    tamanho = r * 1.2f; // tamanho relativo do adesivo

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    // ---------- Desenha o adesivo ----------
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[id_adesivo]);

    glColor4f(1, 1, 1, 1);

    glPushMatrix();
        // Move até o ponto da lateral (esquerda ou direita)
        //glTranslatef(xAdesivo, yAdesivo, zAdesivo);

        // Rotaciona o adesivo para ficar tangente à superfície
        //glRotatef(ladoDireito ? 90 : -90, 0, 0, 1); // orienta o plano
        //glRotatef(90, 0, 1, 0);                     // vira para o lado externo

        glTranslatef(0, R, r + offset); // move ligeiramente para fora

        s = tamanho * 0.5f;
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f(-s, -s, 0);
            glTexCoord2f(1, 0); glVertex3f( s, -s, 0);
            glTexCoord2f(1, 1); glVertex3f( s,  s, 0);
            glTexCoord2f(0, 1); glVertex3f(-s,  s, 0);
        glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_POLYGON_OFFSET_FILL);
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

void marcax(float x, float y, float z, float cy, float cp){
    glPushMatrix();

    // Move até o ponto onde o X será desenhado
    glTranslatef(x, y, z);

    // Pega a matriz atual (modelo-vista)
    GLfloat modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    // Zera a rotação da matriz (mantém apenas a posição)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            modelview[i*4 + j] = (i == j ? 1.0f : 0.0f);
        }
    }

    // Recarrega matriz "sem rotação" → billboard perfeito
    glLoadMatrixf(modelview);

    glScalef(0.5f,0.5f,0.5f);

    glColor4f(1,1,1,0.5f);
    glBegin(GL_POLYGON);
        glVertex3f(-3.25f, -3.0f, 0.0f);
        glVertex3f( 3.25f, -3.0f, 0.0f);
        glVertex3f( 3.25f,  3.0f, 0.0f);
        glVertex3f(-3.25f,  3.0f, 0.0f);
    glEnd();

    glColor4f(1,0,0,1.0f);
    glLineWidth(3.0f);

    // Corpo da câmera (retângulo principal)
    glBegin(GL_LINE_LOOP);
        glVertex3f(-3.0f, -2.0f, 0.0f);
        glVertex3f( 3.0f, -2.0f, 0.0f);
        glVertex3f( 3.0f,  2.0f, 0.0f);
        glVertex3f(-3.0f,  2.0f, 0.0f);
    glEnd();

    // Lente (círculo no centro)
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float theta = 2.0f * M_PI * i / 32;
        float cx = 1.0f * cos(theta);
        float cy = 1.0f * sin(theta);
        glVertex3f(cx, cy, 0.0f);
    }
    glEnd();

    // Flash (retângulo pequeno no topo direito)
    glBegin(GL_LINE_LOOP);
        glVertex3f(1.8f, 2.2f, 0.0f);
        glVertex3f(2.8f, 2.2f, 0.0f);
        glVertex3f(2.8f, 2.8f, 0.0f);
        glVertex3f(1.8f, 2.8f, 0.0f);
    glEnd();

    // Pequeno detalhe no obturador (círculo menor dentro)
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float theta = 2.0f * M_PI * i / 32;
        float cx = 0.5f * cos(theta);
        float cy = 0.5f * sin(theta);
        glVertex3f(cx, cy, 0.0f);
    }
    glEnd();

    glLineWidth(1.0f);
    glPopMatrix();
}
