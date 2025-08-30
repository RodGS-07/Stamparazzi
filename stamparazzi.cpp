#include <SDL2/SDL.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <typeinfo>

#define MOVE_VEL 10.0f
#define CAMERA_SENS 1.0f
#define ANALOG_SENS 1.25f

using namespace std;

int teste = 0;
bool mouse_in = false;
bool pause = false;
bool primeira_pessoa = true;
bool rodando = true;
Uint32 inicio, fim;
float dt;

SDL_Window* window;
SDL_GLContext glContext;
SDL_GameController* game_controller = NULL;
string modo_controle = "PC"; // "PC" (COMPUTADOR) OU "CONT" (CONTROLE)

struct AABB {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
};

struct Sphere {
    float x, y, z;
    float r;
};

// checa esfera x esfera
bool SphereVsSphere(const Sphere &a, const Sphere &b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    float dist2 = dx*dx + dy*dy + dz*dz;
    float rsum = a.r + b.r;
    return dist2 <= (rsum * rsum);
}

// checa esfera x AABB (closest point)
bool SphereVsAABB(const Sphere &s, const AABB &b) {
    float cx = s.x;
    float cy = s.y;
    float cz = s.z;

    // closest point on AABB to sphere center
    float closestX = std::max(b.minX, std::min(cx, b.maxX));
    float closestY = std::max(b.minY, std::min(cy, b.maxY));
    float closestZ = std::max(b.minZ, std::min(cz, b.maxZ));

    float dx = closestX - cx;
    float dy = closestY - cy;
    float dz = closestZ - cz;

    float dist2 = dx*dx + dy*dy + dz*dz;
    return dist2 <= (s.r * s.r);
}

namespace NG{ //Namespace para Informações do Game/Jogo

    enum STATE{
        MENU_PRINCIPAL,
        JOGO_PRINCIPAL,
        PAUSE,
        VITORIA,
        DERROTA
    };
};

namespace NC{ //Namespace para Controles e Comandos
    enum C {
        FRENTE,
        TRAS,
        ESQUERDA,
        DIREITA,
        CIMA,
        BAIXO,
        CAM_ESQUERDA,
        CAM_DIREITA,
        CAM_CIMA,
        CAM_BAIXO,
        SAIR,
        PAUSAR
    }; //comandos

    void traduz_entradas(){
        
    }

    void atualiza_controller(SDL_Event evento){
        if (evento.type == SDL_CONTROLLERDEVICEADDED) {
            if (!game_controller) {
                for(int i = 0; i < SDL_NumJoysticks(); i++){
                    if(SDL_IsGameController(i)){
                        game_controller = SDL_GameControllerOpen(i);
                        break;
                    }
                }
                cout << "Controle conectado!" << endl;
                modo_controle = "CONT"; return;
            }
        } else if (evento.type == SDL_CONTROLLERDEVICEREMOVED) {
            if (game_controller) {
                SDL_GameControllerClose(game_controller);
                game_controller = NULL;
                cout << "Controle desconectado!" << endl;
                modo_controle = "PC"; return;
            }
        }
    }
};

namespace ND{ //Namespace para Desenhos

    enum F{
        CUBO,
        PIRAMIDE,
        ESFERA,
        CILINDRO,
        CONE,
        TORUS,
        BEZIER
    };

    struct XYZ
    {
        float x;
        float y;
        float z;
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
        double u = (double)i / (double)ND::NI;  
        double v = (double)j / (double)ND::NJ;  

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
};

namespace NE{ //Namespace para todas as propriedades comuns a todas as Entidades
    class Entidade{
        public:
            float x, y, z;

            Entidade(float ix, float iy, float iz){
                this->x = ix, this->y = iy, this->z = iz;
            };
            Entidade(){};
    };

    static float distancia_entidades(Entidade e1, Entidade e2){
        return sqrt((e1.x-e2.x)*(e1.x-e2.x)+(e1.y-e2.y)*(e1.y-e2.y)+(e1.z-e2.z)*(e1.z-e2.z));
    }
};

namespace NP{ //Namespace para entitdades que são Polígonos

    using namespace NE;
    using namespace ND;

    class Poligono : public Entidade{
        public:
            int superficie;

            Poligono(float ix, float iy, float iz, int s)
            : Entidade(ix, iy, iz), superficie(s) { }

            virtual bool colide_sphere(const Sphere& s) const = 0;
            virtual ~Poligono() = default;

            void desenha_poligono(int cor){
                if(cor >= 0 and cor <= 12) muda_cor(cor);
                glPushMatrix();
                glTranslatef(this->x,this->y,this->z);
                desenha_superficie(this->superficie);
                glPopMatrix();
            }
    };

    class Cubo : public Poligono{
        public:
            float lado;

            Cubo(float ix, float iy, float iz, float l)
            : Poligono(ix,iy,iz,F::CUBO), lado(l) { }

            bool colide_sphere(const Sphere& s) const override {
                AABB box = { x - lado, y - lado, z - lado, x + lado, y + lado, z + lado };
                return SphereVsAABB(s, box);
            }

    };

    class Esfera : public Poligono{
        public:
            float raio;

            Esfera(float ix, float iy, float iz, float r)
            : Poligono(ix,iy,iz,F::ESFERA), raio(r) { }


            bool colide_sphere(const Sphere& s) const override {
                Sphere s2 = { x, y, z, raio };
                return SphereVsSphere(s, s2);
            }

    };
}

vector<unique_ptr<NP::Poligono>> poligonos;

namespace NJ{ // NJ = Namespace para o Jogador

    using namespace NE;
    using namespace ND;

    class Jogador : public Entidade{
        public:
            float cam_yaw, cam_pitch;
            Sphere mascara;

            Jogador(float ix, float iy, float iz, float cy, float cp){
                Entidade(ix,iy,iz);
                this->cam_yaw = cy, this->cam_pitch = cp;
                this->mascara = {this->x,this->y,this->z,1.0f};
            };
            Jogador(){};

            void desenha_mascara(int stacks = 30, int fatias = 30){
                muda_cor(12);

                for (int i = 0; i < stacks; ++i) {
                    float phi1 = M_PI / 2 - i * (M_PI / stacks);
                    float phi2 = M_PI / 2 - (i + 1) * (M_PI / stacks);

                    glBegin(GL_LINE_LOOP);
                    for (int j = 0; j < fatias; ++j) {
                        float theta1 = j * (2 * M_PI / fatias);
                        float theta2 = (j + 1) * (2 * M_PI / fatias);

                        // Vertex 1 (bottom-left of current quad)
                        float x1 = this->mascara.r * cos(phi2) * sin(theta1) + this->x;
                        float y1 = this->mascara.r * sin(phi2) + this->y;
                        float z1 = this->mascara.r * cos(phi2) * cos(theta1) + this->z;
                        glVertex3f(x1, y1, z1);

                        // Vertex 2 (bottom-right of current quad)
                        float x2 = this->mascara.r * cos(phi2) * sin(theta2) + this->x;
                        float y2 = this->mascara.r * sin(phi2) + this->y;
                        float z2 = this->mascara.r * cos(phi2) * cos(theta2) + this->z;
                        glVertex3f(x2, y2, z2);

                        // Vertex 3 (top-right of current quad)
                        float x3 = this->mascara.r * cos(phi1) * sin(theta2) + this->x;
                        float y3 = this->mascara.r * sin(phi1) + this->y;
                        float z3 = this->mascara.r * cos(phi1) * cos(theta2) + this->z;
                        glVertex3f(x3, y3, z3);

                        // Vertex 4 (top-left of current quad)
                        float x4 = this->mascara.r * cos(phi1) * sin(theta1) + this->x;
                        float y4 = this->mascara.r * sin(phi1) + this->y;
                        float z4 = this->mascara.r * cos(phi1) * cos(theta1) + this->z;
                        glVertex3f(x4, y4, z4);
                    }
                glEnd();
                }
            }

            bool tenta_mover(float dx, float dy, float dz){
                Sphere candidate = this->mascara;
                candidate.x += dx;
                candidate.y += dy;
                candidate.z += dz;

                // testa contra todos os poligonos (use referências para evitar cópia)
                for (const auto& p : poligonos) 
                    if (p->colide_sphere(candidate)) 
                        return false; // colisão detectada => rejeita movimento

                // sem colisão => confirma movimento
                this->x += dx;
                this->y += dy;
                this->z += dz;
                this->mascara = candidate;
                return true;
            }

            void prende_camera(){
                if(cam_yaw < 0.0f) cam_yaw += 360.0f;
                if(cam_yaw > 360.0f) cam_yaw -= 360.0f;
                if(cam_pitch > 90.0f) cam_pitch = 90.0f;
                if(cam_pitch < -90.0f) cam_pitch = -90.0f;
            }

            void move_camera(float dist, float dir, float val = 0.0f){
                if(dir >= 0.0f){
                    float rad = (cam_yaw + dir) * M_PI / 180.0f;
                    float dx = - sin(rad) * dist * dt;//this->x -= sin(rad) * dist * dt;
                    float dz = - cos(rad) * dist * dt;//this->z -= cos(rad) * dist * dt;
                    tenta_mover(dx,0.0f,dz);
                } else {
                    float dy = dist * val * dt;//this->y += dist * val * dt;
                    tenta_mover(0.0f,dy,0.0f);
                }
                
            }

            void controle_camera(float move_vel, float camera_sens){
                if(!pause and !game_controller){
                    int midx = 320, midy = 240, tempx, tempy;
                    SDL_ShowCursor(SDL_DISABLE);
                    SDL_GetMouseState(&tempx, &tempy);
                    cam_yaw += camera_sens * (midx - tempx) * dt;
                    cam_pitch += camera_sens * (midy - tempy) * dt;
                    prende_camera();
                    SDL_WarpMouseInWindow(window,midx,midy);
                    const Uint8* state = SDL_GetKeyboardState(NULL);
                    if(state[SDL_SCANCODE_UP] or state[SDL_SCANCODE_W])
                        if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                            move_camera(move_vel,0.0f);
                    if(state[SDL_SCANCODE_DOWN] or state[SDL_SCANCODE_S])
                        if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                            move_camera(move_vel,180.0f);
                    if(state[SDL_SCANCODE_LEFT] or state[SDL_SCANCODE_A])
                        move_camera(move_vel,90.0f);
                    if(state[SDL_SCANCODE_RIGHT] or state[SDL_SCANCODE_D])
                        move_camera(move_vel,270.0f);
                    if(state[SDL_SCANCODE_LSHIFT] or state[SDL_SCANCODE_RSHIFT])
                        move_camera(move_vel,-1.0f,1.0f);
                    if(state[SDL_SCANCODE_LCTRL] or state[SDL_SCANCODE_RCTRL])
                        move_camera(move_vel,-1.0f,-1.0f);
                } else if(!pause and game_controller) {
                    Sint16 axisRX = SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_RIGHTX);
                    Sint16 axisRY = SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_RIGHTY);
                    if(fabs(axisRX) > 16000.0f) cam_yaw   -= (static_cast<float>(axisRX) / 32767.0f) * ANALOG_SENS;  // multiplica para sensibilidade
                    if(fabs(axisRY) > 16000.0f) cam_pitch -= (static_cast<float>(axisRY) / 32767.0f) * ANALOG_SENS;
                    prende_camera();
                    if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_UP)
                        or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTY) < -16000)
                        if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                            move_camera(move_vel,0.0f);
                    if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN)
                        or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTY) > 16000)
                        if(cam_pitch != 90.0f and cam_pitch != -90.0f)
                            move_camera(move_vel,180.0f);
                    if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT)
                        or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTX) < -16000)
                        move_camera(move_vel,90.0f);
                    if(SDL_GameControllerGetButton(game_controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
                        or SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTX) > 16000)
                        move_camera(move_vel,270.0f);
                    if(SDL_GameControllerGetButton(game_controller,SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
                        move_camera(move_vel,-1.0f,1.0f);
                    if(SDL_GameControllerGetButton(game_controller,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
                        move_camera(move_vel,-1.0f,-1.0f);
                }
                if(primeira_pessoa) {
                    glRotatef(-cam_pitch, 1.0, 0.0, 0.0); 
                    glRotatef(-cam_yaw, 0.0, 1.0, 0.0);
                    glTranslatef(-(this->x),-(this->y),-(this->z));
                } else {
                    gluLookAt(this->x,this->y+25.0f,this->z+25.0f,
                            this->x,this->y,this->z,
                            0.0f,1.0f,0.0f);
                    desenha_mascara();
                }
                this->mascara = {this->x,this->y,this->z,1.0f};
                
            }
    };
};

NJ::Jogador jogador = NJ::Jogador(0.0f,0.0f,0.0f,0.0f,0.0f);

void inicializa_sdl(){
    // Inicializa SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "Erro ao inicializar SDL2: " << SDL_GetError() << endl;
        teste = -1;
    }

    // Cria a janela com contexto OpenGL
    window = SDL_CreateWindow("Stamparazzi",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        cerr << "Erro ao criar janela: " << SDL_GetError() << endl;
        SDL_Quit();
        teste = -1;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        cerr << "Erro ao criar contexto OpenGL: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        teste = -1;
    }

    //Necessário para o meu controle
    SDL_GameControllerAddMapping(
    "030081f4790000000600000000000000,USB Network Joystick,"
    "a:b2,b:b1,x:b3,y:b0,back:b8,start:b9,guide:b12,"
    "leftshoulder:b6,rightshoulder:b7,leftstick:b4,rightstick:b5,"
    "lefttrigger:b10,righttrigger:b11,"
    "dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,"
    "leftx:a0,lefty:a1,rightx:a2,righty:a3,"
    );

    for(int i = 0; i < SDL_NumJoysticks(); i++){
        if(SDL_IsGameController(i)){
            game_controller = SDL_GameControllerOpen(i);
            modo_controle = "CONT";
            break;
        }
    }
}

void inicializa_opengl(){
    // Configuração básica do OpenGL
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 800.0/600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void cria_poligonos(int n){
    poligonos.push_back(make_unique<NP::Cubo>(0.0f,10.0f,-20.0f,2.0f));
    poligonos.push_back(make_unique<NP::Esfera>(10.0f,10.0f,-20.0f,2.0f));
    /*for(int i = 0; i < n; i++){
        poligonos.push_back();
    }*/
}

void loop_jogo(){

    SDL_Event evento;
    inicio = SDL_GetTicks();

    while (rodando) {

        fim = SDL_GetTicks();
        dt = (fim - inicio) / 1000.0f;
        inicio = fim;

        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT) {
                rodando = false;
            }

            NC::atualiza_controller(evento);
            
            if(evento.type == SDL_KEYDOWN){
                if(evento.key.keysym.sym == SDLK_ESCAPE) rodando = false;
                else if(evento.key.keysym.sym == SDLK_p){
                    pause = !pause;
                    SDL_ShowCursor(pause ? SDL_ENABLE : SDL_DISABLE);
                }else if(evento.key.keysym.sym == SDLK_t)
                    primeira_pessoa = !primeira_pessoa;
            }
            if(evento.type == SDL_MOUSEBUTTONDOWN && pause){
                pause = false;
                SDL_ShowCursor(SDL_DISABLE);
            }

            if(evento.type == SDL_CONTROLLERBUTTONDOWN) {
                if(evento.cbutton.button == SDL_CONTROLLER_BUTTON_BACK)
                    rodando = false;
                else if(evento.cbutton.button == SDL_CONTROLLER_BUTTON_START)
                    pause = !pause;
            }
        }

        // Limpa tela
        if(!game_controller and !pause) glClearColor(1.0f,0.0f,0.5f,1.0f);
        else if(!game_controller and pause) glClearColor(0.5f,0.0f,0.25f,1.0f);
        else if(!pause) glClearColor(0.5f,0.0f,0.5f,1.0f);
        else glClearColor(0.25f,0.0f,0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        // Controla câmera
		jogador.controle_camera(MOVE_VEL, CAMERA_SENS);

        // Desenha chão
		glPushMatrix();
			glTranslatef(0,-1,0);
			glScalef(100,-0.1,100);
        	ND::desenha_chao();
		glPopMatrix();

        /*for(int i = 0; i < 26; i+=2){
            ND::muda_cor(i/2);
            glPushMatrix();
                glTranslatef(-20+i*2,5,-15);
                ND::desenha_superficie(ND::F::CUBO);
            glPopMatrix();
        }

        for(int i = 0; i < 6; i++){
            ND::muda_cor(i);
            glPushMatrix();
                glTranslatef(i*10,5,-30);
                ND::desenha_superficie(i);
            glPopMatrix();
        }*/

        for (const auto& p : poligonos)
            p->desenha_poligono(1);

        // Atualiza tela
        SDL_GL_SwapWindow(window);
    }
}

void finaliza_sdl(){
    if(game_controller) {
        SDL_GameControllerClose(game_controller);
        game_controller = NULL;
    }
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    inicializa_sdl(); if(teste == -1) return teste;
    inicializa_opengl();

	SDL_ShowCursor(SDL_ENABLE);

    cria_poligonos(2);

    loop_jogo();

    finaliza_sdl();

    return 0;
}