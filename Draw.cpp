// Código para Desenhos
#include "Draw.h"
#include "Linear.h"
#include "Textura.h"
#include <GL/glut.h>
#include <cmath>
#include <vector>

using namespace std;

void muda_cor(int c){
    glColor3f(cores[c][0],cores[c][1],cores[c][2]);
}

void desenha_cubo(float lado, int id) {
    XYZ normal;

    glBegin(GL_QUADS);

    normal = Normal({-lado,-lado,lado},{lado,-lado,lado},{lado,lado,lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(-lado, -lado,  lado);
    glVertex3f( lado, -lado,  lado);
    glVertex3f( lado,  lado,  lado);
    glVertex3f(-lado,  lado,  lado);

    normal = Normal({-lado,-lado,-lado},{-lado,lado,-lado},{lado,lado,-lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(-lado, -lado, -lado);
    glVertex3f(-lado,  lado, -lado);
    glVertex3f( lado,  lado, -lado);
    glVertex3f( lado, -lado, -lado);

    normal = Normal({-lado,-lado,-lado},{-lado,-lado,lado},{-lado,lado,lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(-lado, -lado, -lado);
    glVertex3f(-lado, -lado,  lado);
    glVertex3f(-lado,  lado,  lado);
    glVertex3f(-lado,  lado, -lado);

    normal = Normal({lado,-lado,-lado},{lado,lado,-lado},{lado,lado,lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(lado, -lado, -lado);
    glVertex3f(lado,  lado, -lado);
    glVertex3f(lado,  lado,  lado);
    glVertex3f(lado, -lado,  lado);

    normal = Normal({-lado,lado,-lado},{-lado,lado,lado},{lado,lado,lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(-lado, lado, -lado);
    glVertex3f(-lado, lado,  lado);
    glVertex3f( lado, lado,  lado);
    glVertex3f( lado, lado, -lado);

    normal = Normal({-lado,-lado,-lado},{lado,-lado,-lado},{lado,-lado,lado});
    glNormal3f(normal.x,normal.y,normal.z);
    glVertex3f(-lado, -lado, -lado);
    glVertex3f( lado, -lado, -lado);
    glVertex3f( lado, -lado,  lado);
    glVertex3f(-lado, -lado,  lado);

    glEnd();

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    float adesivoTamanho = lado * 0.3f; // 30% da face
    float offset = lado + 0.01f;       // ligeiramente à frente da face

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[id]);

    glColor4f(1, 1, 1, 1); // mantém as cores originais da textura

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-adesivoTamanho, -adesivoTamanho, offset);
        glTexCoord2f(1, 0); glVertex3f( adesivoTamanho, -adesivoTamanho, offset);
        glTexCoord2f(1, 1); glVertex3f( adesivoTamanho,  adesivoTamanho, offset);
        glTexCoord2f(0, 1); glVertex3f(-adesivoTamanho,  adesivoTamanho, offset);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_POLYGON_OFFSET_FILL);
}

void desenha_piramide(float base, float altura, int id){
    XYZ normal;
    float h = altura;
    float b = base / 2.0f; // metade do tamanho da base

    // --- Base (quadrado no plano y=0) ---
    normal = Normal({-b,-b,-b},{b,-b,-b},{b,-b,b});
    glNormal3f(normal.x,normal.y,normal.z);
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

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    float offset = 0.001f;                // leve afastamento da superfície

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[id]);

    glColor4f(1, 1, 1, 1);

    glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 0); glVertex3f(-(b/2) * 0.5f, (-b + offset) * 0.25f,  b*0.65f);
        glTexCoord2f(1, 0); glVertex3f( (b/2) * 0.5f, (-b + offset) * 0.25f,  b*0.65f);
        glTexCoord2f(0.5, 1); glVertex3f( 0,  (h*0.4f) * 0.15f,      b*0.4375f);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_POLYGON_OFFSET_FILL);
}

void desenha_esfera(float raio, int fatias, int stacks, int id){
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

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    float adesivoRaio = raio * 1.001f; // ligeiramente menor (pra evitar z-fighting)
    float adesivoAngulo = M_PI / 6;    // “tamanho” angular do adesivo (~30°)
    float fatiasAdesivo = 20;          // resolução do adesivo

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[id]);

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

void desenha_cilindro(float raio, float altura, int fatias, int stacks, bool tampas, int id){
    float half = altura / 2.0f;
    XYZ normal;

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

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f);
        // --- Adesivo na tampa superior ---
        float adesivoRaio = raio * 0.4f; // menor que o raio da tampa
        float offset = half + 0.01f;    // um pouquinho acima da tampa

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texID[id]);

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

        glDisable(GL_TEXTURE_2D);

        glDisable(GL_POLYGON_OFFSET_FILL);
    }
}

void desenha_cone(float raio, float altura, int fatias, int id){
    float half = altura / 2.0f;
    XYZ normal;

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

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);
    // ---------------- Adesivo na lateral ----------------
    // O adesivo será colado no lado "frontal" do cone (direção +Z)
    float adesivoAltura = altura * 0.3f;  // fração da altura
    float adesivoLargura = raio * 0.8f;   // fração da largura
    float zOffset = half - adesivoAltura * 0.8f; // mais próximo do ápice
    float rOffset = raio * 0.98f;         // quase colado à lateral

    // Calcula ângulo médio da frente (+Z)
    float theta = 0.0f; // direção frontal (Z positivo)
    float xDir = rOffset * sin(theta);
    float yDir = rOffset * cos(theta);

    // Posição média do adesivo na superfície
    float adesivoZ = -half + adesivoAltura * 0.5f;

    // Configura textura
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[id]);

    glColor4f(1, 1, 1, 1); // mantém cores da textura

    // Coloca o adesivo orientado tangente à superfície
    glPushMatrix();
        // Move o adesivo até a posição na lateral do cone
        glTranslatef(xDir, yDir - 0.26f, adesivoZ);

        // Calcula ângulo de inclinação da superfície do cone
        float slopeAngle = 63.75f; //atan(raio / altura) * 180.0f / M_PI + 36.5f;

        // Rotaciona o adesivo para acompanhar a inclinação do cone
        glRotatef(-slopeAngle, 1, 0, 0);

        // Rotaciona para ficar de frente (+Z)
        //glRotatef(90, 0, 1, 0);

        float s = adesivoLargura * 0.5f;
        float h = adesivoAltura * 0.5f;

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

void desenha_torus(float R, float r, int fatias, int stacks, int id){
    vector<vector<float>> mat(3,vector<float> (3));
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

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);
    // ---------- Define posição e lado do adesivo ----------
    bool ladoDireito = 1; // 1 = direito

    float angulo = ladoDireito ? M_PI / 2 : -M_PI / 2; // +90° ou -90°
    float xAdesivo = R * cos(angulo);
    float yAdesivo = R * sin(angulo);
    float zAdesivo = 0.0f;

    float offset = 0.002f; // evita z-fighting
    float tamanho = r * 1.2f; // tamanho relativo do adesivo

    // ---------- Desenha o adesivo ----------
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[id]);

    glColor4f(1, 1, 1, 1);

    glPushMatrix();
        // Move até o ponto da lateral (esquerda ou direita)
        //glTranslatef(xAdesivo, yAdesivo, zAdesivo);

        // Rotaciona o adesivo para ficar tangente à superfície
        //glRotatef(ladoDireito ? 90 : -90, 0, 0, 1); // orienta o plano
        //glRotatef(90, 0, 1, 0);                     // vira para o lado externo

        glTranslatef(0, R, r + offset); // move ligeiramente para fora

        float s = tamanho * 0.5f;
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

/*double BezierBlend(int k,double mu, int n) {
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

void entrada_inpus(int forma, int i, int j){
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
            inpu[i][j].x = R * sin(phi) * cos(theta);
            inpu[i][j].y = R * sin(phi) * sin(theta);
            inpu[i][j].z = R * cos(phi);
            break;
        
        case CILINDRO: 
            R = 2.0f; // raio
            H = 4.0f; // altura
            inpu[i][j].x = R * cos(theta);
            inpu[i][j].y = R * sin(theta);
            inpu[i][j].z = (v - 0.5f) * H;
            break;
        
        case CONE: 
            H = 4.0f;       // altura
            Rmax = 2.0f;    // raio da base
            R = (1.0f - v) * Rmax;
            inpu[i][j].x = R * cos(theta);
            inpu[i][j].y = R * sin(theta);
            inpu[i][j].z = v * H;
            break;
        
        case TORUS: 
            R = 3.0f; // raio maior (centro até tubo)
            r = 1.0f; // raio menor (espessura do tubo)
            inpu[i][j].x = (R + r * cos(phi)) * cos(theta);
            inpu[i][j].y = (R + r * cos(phi)) * sin(theta);
            inpu[i][j].z = r * sin(phi);
            break;
        
        case BEZIER: 
            // Exemplo simples: grid de pontos com ondulação
            inpu[i][j].x = i;
            inpu[i][j].y = j;
            inpu[i][j].z = 2.0f * sin(i * 0.5f) * cos(j * 0.5f);
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
                entrada_inpus(forma,i,j);
            }
        }
    }
}*/


//cálculos necessários para configurar a normal dos vértices da superfície
/*XYZ calculaNormal(XYZ u, XYZ v) {
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
}*/


/*void Surface(int forma) {
    if(forma != CUBO and forma != PIRAMIDE){
        int i,j,ki,kj;
        double mui,muj,bi,bj;
        for (i=0;i<RESOLUTIONI;i++) {
            mui = i / (double)(RESOLUTIONI-1);
            for (j=0;j<RESOLUTIONJ;j++) {
                muj = j / (double)(RESOLUTIONJ-1);
                outpu[i][j].x = 0;
                outpu[i][j].y = 0;
                outpu[i][j].z = 0;
                for (ki=0;ki<=NI;ki++) {
                    bi = BezierBlend(ki,mui,NI);
                    for (kj=0;kj<=NJ;kj++) {
                    bj = BezierBlend(kj,muj,NJ);
                    outpu[i][j].x += (inpu[ki][kj].x * bi * bj);
                    outpu[i][j].y += (inpu[ki][kj].y * bi * bj);
                    outpu[i][j].z += (inpu[ki][kj].z * bi * bj);
                    }
                }
            }
        }
        for(i=0;i<RESOLUTIONI-1;i++){
            for(j=0;j<RESOLUTIONJ-1;j++){
                XYZ u = {outpu[i+1][j].x - outpu[i][j].x, outpu[i+1][j].y - outpu[i][j].y, outpu[i+1][j].z - outpu[i][j].z};
                XYZ v = {outpu[i+1][j+1].x - outpu[i][j].x, outpu[i+1][j+1].y - outpu[i][j].y, outpu[i+1][j+1].z - outpu[i][j].z};
                XYZ normal = calculaNormal(u,v);
                glBegin(GL_QUADS);
                    glNormal3f(normal.x,normal.y,normal.z); glVertex3f(outpu[i][j].x,outpu[i][j].y,outpu[i][j].z);
                    glNormal3f(normal.x,normal.y,normal.z); glVertex3f(outpu[i+1][j].x,outpu[i+1][j].y,outpu[i+1][j].z);
                    glNormal3f(normal.x,normal.y,normal.z); glVertex3f(outpu[i+1][j+1].x,outpu[i+1][j+1].y,outpu[i+1][j+1].z);
                    glNormal3f(normal.x,normal.y,normal.z); glVertex3f(outpu[i][j+1].x,outpu[i][j+1].y,outpu[i][j+1].z);
                glEnd();
            }
        }
    } else if(forma == CUBO)
        desenha_cubo();
    else {
        desenha_piramide();
    }
}*/