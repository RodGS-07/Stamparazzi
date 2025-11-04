// Códigos para carregamento e uso de Texturas
#include "Textura.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Linear.h"
#include "Draw.h"
#include <GL/glut.h>
#include <GL/glext.h>
#include <iostream>
#include <cmath>

using namespace std;

GLuint texID[QTDTEXT];  // ID para as texturas

const char* textureFileNames[QTDTEXT] = {   //nomes dos arquivos das texturas
    "chatgpt_chao.png",
    "chatgpt_parede3.png",
    "chatgpt_teto.png",
    "adesivo_photopea01.png",
    "adesivo_photopea02.png",
    "adesivo_photopea03.png",
    "adesivo_photopea04.png",
    "adesivo_photopea05.png",
    "adesivo_photopea06.png",
    "adesivo_photopea07.png",
    "adesivo_photopea08.png",
    "adesivo_photopea09.png",
    "adesivo_photopea10.png",
    "adesivo_photopea11.png",
    "adesivo_photopea12.png",
    "adesivo_photopea13.png",
    "adesivo_photopea14.png",
    "adesivo_photopea15.png",
    "adesivo_photopea16.png",
    "adesivo_photopea17.png",
    "adesivo_photopea18.png",
    "adesivo_photopea19.png",
    "adesivo_photopea20.png"
};

void drawSquare(XYZ s,XYZ t){
    // glNormal3f(0,1,0);//normal apontando para cima
    // glBegin(GL_QUADS);//quadrado de cima
    //     glTexCoord2f(s.x,s.y);
    //     glVertex3f(-0.5,0,-0.5);
    //     glTexCoord2f(s.x,t.y);
    //     glVertex3f(-0.5,0,0.5);
    //     glTexCoord2f(t.x,t.y);
    //     glVertex3f( 0.5,0,0.5);
    //     glTexCoord2f(t.x,s.y);
    //     glVertex3f( 0.5,0,-0.5);
    // glEnd();
}

void drawSphere(GLfloat r = 1.0){
    // static const double step = M_PI/10;
    // static const double lim = 2*M_PI;
    // double i,j;
    // XYZ v1,v2,v3,v4,n;
    // //desenha esfera de raio 1
    // glPushMatrix();
    // glScalef(r,r,r);
    // for(i = 0; i < lim/2; i += step){
    //     for(j = 0; j < lim; j += step){
    //         v1 = XYZ(cos(i   )*sin(j   ),sin(i   )*sin(j   ),cos(j   ));
    //         v2 = XYZ(cos(i   )*sin(j+step),sin(i   )*sin(j+step),cos(j+step));
    //         v3 = XYZ(cos(i+step)*sin(j+step),sin(i+step)*sin(j+step),cos(j+step));
    //         v4 = XYZ(cos(i+step)*sin(j   ),sin(i+step)*sin(j   ),cos(j   ));
    //         glBegin(GL_QUADS);//quadrado de cima
    //             glTexCoord2f(i/M_PI,j/lim);
    //             glNormal3f(v1.x,v1.y,v1.z);
    //             glVertex3f(v1.x,v1.y,v1.z);
                
    //             glTexCoord2f(i/M_PI,(j+step)/lim);
    //             glNormal3f(v2.x,v2.y,v2.z);
    //             glVertex3f(v2.x,v2.y,v2.z);
                
    //             glTexCoord2f((i+step)/M_PI,(j+step)/lim);
    //             glNormal3f(v3.x,v3.y,v3.z);
    //             glVertex3f(v3.x,v3.y,v3.z);
                
    //             glTexCoord2f((i+step)/M_PI,j/lim);
    //             glNormal3f(v4.x,v4.y,v4.z);
    //             glVertex3f(v4.x,v4.y,v4.z);
    //         glEnd();
    //     }
    // }

    // glPopMatrix();
}

void CarregaTexturas(){
    // stbi_set_flip_vertically_on_load(true);        // carrega com Y certo
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);         // evita leitura desalinhada

    // glGenTextures(QTDTEXT, texID);
    // for(int i=0;i<QTDTEXT;i++){
    //     int w,h,channels;
    //     unsigned char* data = stbi_load(textureFileNames[i], &w, &h, &channels, 4);
    //     if(!data){
    //         fprintf(stderr,"Erro load %s\n", textureFileNames[i]);
    //         continue;
    //     }

    //     glBindTexture(GL_TEXTURE_2D, texID[i]);

    //     // essencial: parâmetros aplicados *imediatamente* após bind e antes de gerar mipmaps
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);

    //     stbi_image_free(data);
    // }
    // glBindTexture(GL_TEXTURE_2D, 0);
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data;

    glGenTextures(QTDTEXT, texID);

    for(int i = 0; i < QTDTEXT;i++)
    {
            glGenTextures(1, &texID[i]);
            glBindTexture(GL_TEXTURE_2D, texID[i]);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            stbi_set_flip_vertically_on_load(true);

            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            // load and generate the texture

            data = stbi_load(textureFileNames[i], &width, &height, &nrChannels, 0);
            if (data)
            {
                gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
                if (nrChannels == 3)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                else if (nrChannels == 4)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
            else
            {
                cerr << "Failed to load texture" << endl;
            }
            stbi_image_free(data);
    }
}

void TransformaParede(int i, int s){
    // float h = s / 2.0f;
    // switch (i){
    //     case 0: glTranslatef( h, 0, 0); glRotatef(90, 0, 1, 0); break; // Right
    //     case 1: glTranslatef(-h, 0, 0); glRotatef(-90, 0, 1, 0); break; // Left
    //     case 2: glTranslatef(0, -h, 0); glRotatef(270,0,1,0); glRotatef(90, 1, 0, 0); break;  // Top
    //     case 3: glTranslatef(0,  h, 0); glRotatef(270,0,1,0); glRotatef(-90, 1, 0, 0); break; // Bottom
    //     case 4: glTranslatef(0, 0, -h); glRotatef(180, 0, 1, 0); break; // Back
    //     case 5: glTranslatef(0, 0,  h); break; // Front
    // }
}

void DesenhaSkybox(){
    //M::defineMaterial(M::MAT::PEARL);
    // int s=150; XYZ n;
    // n = Normal({-0.5,-0.5,0},{-0.5,0.5,0},{0.5,0.5,0});
    // glNormal3f(n.x,n.y,n.z);
    // for(int i = 0; i < 6; i++){
    //     glPushMatrix();
    //         glEnable(GL_TEXTURE_2D);
    //         glBindTexture(GL_TEXTURE_2D,texID[i+3]);
    //         glRotatef(180,0,0,1);
    //         TransformaParede(i,s);
    //         glScalef(s,s,1);
    //         glBegin(GL_QUADS);
    //             glTexCoord2d(0,0); glVertex3f(-0.5,-0.5,0);
    //             glTexCoord2d(0,1); glVertex3f(-0.5,0.5,0);
    //             glTexCoord2d(1,1); glVertex3f(0.5,0.5,0);
    //             glTexCoord2d(1,0); glVertex3f(0.5,-0.5,0);
    //         glEnd();
    //         glDisable(GL_TEXTURE_2D);
    //     glPopMatrix();
    // }
}

void desenha_paredes() {
    muda_cor(10);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[1]); // parede

    float tamanho = 100.0f;

    glBegin(GL_QUADS);
    // Parede frente
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(-tamanho, -tamanho, tamanho);
    glTexCoord2f(5, 0); glVertex3f(tamanho, -tamanho, tamanho);
    glTexCoord2f(5, 5); glVertex3f(tamanho, tamanho, tamanho);
    glTexCoord2f(0, 5); glVertex3f(-tamanho, tamanho, tamanho);
    glEnd();

    glBegin(GL_QUADS);
    // Parede traseira
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-tamanho, -tamanho, -tamanho);
    glTexCoord2f(5, 0); glVertex3f(tamanho, -tamanho, -tamanho);
    glTexCoord2f(5, 5); glVertex3f(tamanho, tamanho, -tamanho);
    glTexCoord2f(0, 5); glVertex3f(-tamanho, tamanho, -tamanho);
    glEnd();

    // Parede direita
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(tamanho, -tamanho, tamanho);
    glTexCoord2f(5, 0); glVertex3f(tamanho, -tamanho, -tamanho);
    glTexCoord2f(5, 5); glVertex3f(tamanho, tamanho, -tamanho);
    glTexCoord2f(0, 5); glVertex3f(tamanho, tamanho, tamanho);
    glEnd();

    // Parede esquerda
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(-tamanho, -tamanho, -tamanho);
    glTexCoord2f(5, 0); glVertex3f(-tamanho, -tamanho, tamanho);
    glTexCoord2f(5, 5); glVertex3f(-tamanho, tamanho, tamanho);
    glTexCoord2f(0, 5); glVertex3f(-tamanho, tamanho, -tamanho);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texID[2]); // teto
    // Teto superior
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0); glVertex3f(-tamanho, tamanho, -tamanho);
    glTexCoord2f(5, 0); glVertex3f(-tamanho, tamanho, tamanho);
    glTexCoord2f(5, 5); glVertex3f(tamanho, tamanho, tamanho);
    glTexCoord2f(0, 5); glVertex3f(tamanho, tamanho, -tamanho);
    glEnd();

    // Teto inferior
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(-tamanho, -tamanho, -tamanho);
    glTexCoord2f(5, 0); glVertex3f(-tamanho, -tamanho, tamanho);
    glTexCoord2f(5, 5); glVertex3f(tamanho, -tamanho, tamanho);
    glTexCoord2f(0, 5); glVertex3f(tamanho, -tamanho, -tamanho);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void desenha_chao() {

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[0]); // textura do chão
    muda_cor(9);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1, 1, 1); // evita tingir a textura

    glBegin(GL_QUADS);

    // Frente
    glNormal3f(0,0,1);
    glTexCoord2f(0,0); glVertex3f(-1, -1,  1);
    glTexCoord2f(1,0); glVertex3f( 1, -1,  1);
    glTexCoord2f(1,1); glVertex3f( 1,  1,  1);
    glTexCoord2f(0,1); glVertex3f(-1,  1,  1);

    // Trás
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0); glVertex3f(-1, -1, -1);
    glTexCoord2f(1,0); glVertex3f( 1, -1, -1);
    glTexCoord2f(1,1); glVertex3f( 1,  1, -1);
    glTexCoord2f(0,1); glVertex3f(-1,  1, -1);

    // Esquerda
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0); glVertex3f(-1, -1, -1);
    glTexCoord2f(1,0); glVertex3f(-1, -1,  1);
    glTexCoord2f(1,1); glVertex3f(-1,  1,  1);
    glTexCoord2f(0,1); glVertex3f(-1,  1, -1);

    // Direita 
    glNormal3f(1,0,0);
    glTexCoord2f(0,0); glVertex3f( 1, -1, -1);
    glTexCoord2f(1,0); glVertex3f( 1, -1,  1);
    glTexCoord2f(1,1); glVertex3f( 1,  1,  1);
    glTexCoord2f(0,1); glVertex3f( 1,  1, -1);

    // Topo (chão visível)
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3f(-1, 1, -1);
    glTexCoord2f(4,0); glVertex3f( 1, 1, -1);
    glTexCoord2f(4,4); glVertex3f( 1, 1,  1);
    glTexCoord2f(0,4); glVertex3f(-1, 1,  1);

    // Base 
    glNormal3f(0,-1,0);
    glTexCoord2f(0,0); glVertex3f(-1, -1, -1);
    glTexCoord2f(4,0); glVertex3f( 1, -1, -1);
    glTexCoord2f(4,4); glVertex3f( 1, -1,  1);
    glTexCoord2f(0,4); glVertex3f(-1, -1,  1);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}