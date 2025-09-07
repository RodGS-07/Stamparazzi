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
bool flash_ativo = false;
float flash_alpha = 0.0f;
bool rodando = true;
Uint32 inicio, fim;
float dt;
const Uint8* state;

SDL_Window* window;
SDL_GLContext glContext;
SDL_GameController* game_controller = NULL;
//string modo_controle = "PC"; // "PC" (COMPUTADOR) OU "CONT" (CONTROLE)



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
                //modo_controle = "CONT"; return;
            }
        } else if (evento.type == SDL_CONTROLLERDEVICEREMOVED) {
            if (game_controller) {
                SDL_GameControllerClose(game_controller);
                game_controller = NULL;
                cout << "Controle desconectado!" << endl;
                //modo_controle = "PC"; return;
            }
        }
    }
};

namespace NL{ //Namespace para operações de álgebra linear envolvendo vetores

    struct XYZ{//representa pontos e vetores
        float x, y, z;
        XYZ() 
        : x(0.0f), y(0.0f), z(0.0f) { }

        XYZ(float ix, float iy, float iz)
        : x(ix), y(iy), z(iz) { }
    };

    GLfloat operator!(const XYZ& v){//Norma
        return (sqrt(v.x*v.x+v.y*v.y+v.z*v.z));
    }
    XYZ operator-(const XYZ& o,const XYZ& d){//Diferença de Vetores(ou pontos)
        return (XYZ(o.x-d.x,o.y-d.y,o.z-d.z));
    }
    XYZ operator+(const XYZ& o,const XYZ& d){//Soma de Vetores(ou pontos)
        return (XYZ(d.x+o.x,d.y+o.y,d.z+o.z));
    }
    GLfloat Escalar(const XYZ& u,const XYZ& v){//Produto Escalar entre dois vetores
        return u.x*v.x+u.y*v.y+u.z*v.z;
    }
    XYZ operator*(const XYZ& u,const XYZ& v){//Produto Vetorial
        return (XYZ(u.y*v.z-u.z*v.y,u.z*v.x-u.x*v.z,u.x*v.y-u.y*v.x));
    }
    XYZ operator*(const XYZ& u, GLfloat lu){//Produto de um vetor por um escalar
        return XYZ(u.x*lu,u.y*lu,u.z*lu);
    }
    XYZ operator/(const XYZ& u, GLfloat lu){//Divisao de um vetor por um escalar
        return XYZ(u.x/lu,u.y/lu,u.z/lu);
    }
    XYZ Normal(XYZ Pa, XYZ Pb, XYZ Pc){//Normal entre dois vetores, com um ponto em comum
        XYZ n = (Pb - Pa)*(Pc - Pa);
        return (n/(!n));
    }
    GLfloat Arccos(XYZ Pa, XYZ Pb){//Arco cosseno entre dois vetores
        GLfloat e = Escalar(Pa,Pb);
        return acos(e/(!Pa*!Pb));
    }

}

namespace NB{ //Namespace para Bounding Boxes e Colisões

    using namespace NL;

    struct AABB {
        XYZ min, max;
    };

    struct Sphere {
        XYZ c;
        float r;
    };

    bool AABBvsAABB(const AABB& a, const AABB& b) {
        return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&  // sobreposição em X
            (a.min.y <= b.max.y && a.max.y >= b.min.y) &&  // sobreposição em Y
            (a.min.z <= b.max.z && a.max.z >= b.min.z);    // sobreposição em Z
    }

    // checa esfera x esfera
    bool SphereVsSphere(const Sphere &a, const Sphere &b) {
        float dx = a.c.x - b.c.x;
        float dy = a.c.y - b.c.y;
        float dz = a.c.z - b.c.z;
        float dist2 = dx*dx + dy*dy + dz*dz;
        float rsum = a.r + b.r;
        return dist2 <= (rsum * rsum);
    }

    // checa esfera x AABB (closest point)
    bool SphereVsAABB(const Sphere &s, const AABB &b) {
        float cx = s.c.x;
        float cy = s.c.y;
        float cz = s.c.z;

        // closest point on AABB to sphere center
        float closestX = max(b.min.x, min(cx, b.max.x));
        float closestY = max(b.min.y, min(cy, b.max.y));
        float closestZ = max(b.min.z, min(cz, b.max.z));

        float dx = closestX - cx;
        float dy = closestY - cy;
        float dz = closestZ - cz;

        float dist2 = dx*dx + dy*dy + dz*dz;
        return dist2 <= (s.r * s.r);
    }

    inline float Length2(const XYZ&a){ return Escalar(a,a); }
    inline float Clamp(float v,float mn,float mx){ return v<mn?mn:(v>mx?mx:v); }

    XYZ ClosestPointOnSegment(const XYZ& A,const XYZ& B,const XYZ& P){
        XYZ AB = B - A;
        float t = Escalar(P - A, AB) / Length2(AB);
        t = Clamp(t,0.0f,1.0f);
        return A + AB*t;
    }

    struct Capsule {
        XYZ A,B; // extremos do segmento central
        float r;  // raio
    };

    // distância entre dois segmentos (Ericson, ch.5.1.9)
    float SegmentSegmentDist2(const XYZ& A0,const XYZ& A1,
                            const XYZ& B0,const XYZ& B1,
                            float& s,float& t){
        XYZ u = A1 - A0;
        XYZ v = B1 - B0;
        XYZ w = A0 - B0;
        float a = Escalar(u,u);
        float b = Escalar(u,v);
        float c = Escalar(v,v);
        float d = Escalar(u,w);
        float e = Escalar(v,w);
        float D = a*c - b*b;
        s = D < 1e-6f ? 0.0f : Clamp((b*e - c*d)/D,0.0f,1.0f);
        t = (b*s + e)/c;
        if(t<0){t=0;s=Clamp(-d/a,0.0f,1.0f);}
        else if(t>1){t=1;s=Clamp((b-d)/a,0.0f,1.0f);}
        XYZ dP = (A0 + u*s) - (B0 + v*t);
        return Length2(dP);
    }

    bool CapsuleVsCapsule(const Capsule& c1,const Capsule& c2){
        float s,t;
        float dist2 = SegmentSegmentDist2(c1.A,c1.B,c2.A,c2.B,s,t);
        float rsum = c1.r + c2.r;
        return dist2 <= rsum*rsum;
    }

    struct Cylinder {
        XYZ base;   // centro da base
        XYZ axis;   // vetor normalizado do eixo
        float h;     // altura
        float R;     // raio
    };

    bool SphereVsCylinder(const Sphere& s,const Cylinder& cyl){
        // projeção do centro da esfera no eixo
        float t = Escalar(s.c - cyl.base, cyl.axis);
        t = Clamp(t,0.0f,cyl.h);
        XYZ Q = cyl.base + cyl.axis*t;
        XYZ d = s.c - Q;
        float dist2 = Length2(d);
        return dist2 <= (s.r + cyl.R)*(s.r + cyl.R);
    }

    struct ConeBound {
        XYZ apex;   // vértice
        XYZ axis;   // direção (unitário, do ápice à base)
        float h;     // altura
        float R;     // raio da base
    };

    bool PointInConeBound(const XYZ& P,const ConeBound& cone){
        XYZ v = P - cone.apex;
        float t = Escalar(v,cone.axis);
        if(t<0 || t>cone.h) return false;
        float r = (t/cone.h)*cone.R; // raio local
        XYZ proj = cone.apex + cone.axis*t;
        float dist2 = Length2(P - proj);
        return dist2 <= r*r;
    }

    // -----------------------------------------------------------
    // Closest point on triangle (Ericson) + Sphere vs Triangle
    // -----------------------------------------------------------
    XYZ ClosestPointOnTriangle(const XYZ& A, const XYZ& B, const XYZ& C, const XYZ& P) {
        XYZ AB = B - A;
        XYZ AC = C - A;
        XYZ AP = P - A;

        float d1 = Escalar(AB, AP);
        float d2 = Escalar(AC, AP);
        if (d1 <= 0.0f && d2 <= 0.0f) return A; // barycentric (1,0,0)

        XYZ BP = P - B;
        float d3 = Escalar(AB, BP);
        float d4 = Escalar(AC, BP);
        if (d3 >= 0.0f && d4 <= d3) return B; // barycentric (0,1,0)

        float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
            float v = d1 / (d1 - d3);
            return A + AB * v; // on AB
        }

        XYZ CP = P - C;
        float d5 = Escalar(AB, CP);
        float d6 = Escalar(AC, CP);
        if (d6 >= 0.0f && d5 <= d6) return C; // barycentric (0,0,1)

        float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
            float w = d2 / (d2 - d6);
            return A + AC * w; // on AC
        }

        float va = d3 * d6 - d5 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return B + (C - B) * w; // on BC
        }

        // inside face region
        float denom = 1.0f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        return A + AB * v + AC * w;
    }

    bool SphereVsTriangle(const Sphere& s, const XYZ& A, const XYZ& B, const XYZ& C) {
        XYZ cp = ClosestPointOnTriangle(A,B,C,s.c);
        XYZ d = cp - s.c;
        return Length2(d) <= (s.r * s.r);
    }

    // -----------------------------------------------------------
    // Sphere vs Cone bounded (approx / robust test)
    // cone.axis must be normalized, cone.apex at apex, h = height, R = base radius
    // -----------------------------------------------------------
    bool SphereVsCone(const Sphere& s, const ConeBound& cone) {
        // vector from apex to sphere center
        XYZ v = s.c - cone.apex;
        float t = Escalar(v, cone.axis); // projection along axis

        // case 1: sphere center before apex (check apex point)
        if (t <= 0.0f) {
            // distance to apex point
            XYZ d = s.c - cone.apex;
            return Length2(d) <= (s.r * s.r);
        }

        // case 2: beyond base plane -> check disk of base (center at apex + axis*h)
        if (t >= cone.h) {
            XYZ baseCenter = cone.apex + cone.axis * cone.h;
            XYZ d = s.c - baseCenter;
            float dist2 = Length2(d);
            float rsum = s.r + cone.R;
            return dist2 <= (rsum * rsum);
        }

        // case 3: inside slab [0,h] -> check radial distance vs cone local radius
        XYZ proj = cone.apex + cone.axis * t;
        XYZ perp = s.c - proj;
        float distPerp2 = Length2(perp);

        float localR = (t / cone.h) * cone.R; // linear interpolation from apex(0) to base(R)
        float rsum = localR + s.r;
        return distPerp2 <= (rsum * rsum);
    }
}

namespace ND{ //Namespace para Desenhos

    using namespace NL;

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

    using namespace NL;

    class Entidade{
        protected:
            XYZ pos;

        public:
            Entidade(float ix, float iy, float iz){
                this->pos.x = ix, this->pos.y = iy, this->pos.z = iz;
            };
            Entidade() : pos({0.0f,0.0f,0.0f}) {};

            float getX() const { return pos.x; }
            float getY() const { return pos.y; }
            float getZ() const { return pos.z; }
            void setX(float ix) { pos.x = ix; }
            void setY(float iy) { pos.y = iy; }
            void setZ(float iz) { pos.z = iz; }
    };

    static float distancia_entidades(const Entidade& e1, const Entidade& e2){
        float dx = e1.getX()-e2.getX();
        float dy = e1.getY()-e2.getY();
        float dz = e1.getZ()-e2.getZ();
        return sqrt(dx*dx + dy*dy + dz*dz);
    }
};

namespace NA{ //Namespace para Entidades que são adesivos

    using namespace NE;
    using namespace ND;

    class Adesivo : public Entidade{
        public:
            XYZ normal;

            Adesivo(float ix, float iy, float iz, XYZ n)
            : Entidade(ix, iy, iz), normal(n) { }

            void desenha_adesivo(){
                muda_cor(10);
                glPushMatrix();
                glTranslatef(this->pos.x,this->pos.y,this->pos.z);
                glScalef(0.25f,0.25f,0.25f);
                desenha_cilindro();
                glPopMatrix();
            }
    };
};

NA::Adesivo a = NA::Adesivo(-5.0f,5.0f,10.0f,{0,0,1});

namespace NP{ //Namespace para entidades que são Polígonos

    using namespace NB;
    using namespace NE;
    using namespace ND;

    class Poligono : public Entidade{
        public:
            int superficie;

            Poligono(float ix, float iy, float iz, int s)
            : Entidade(ix, iy, iz), superficie(s) { }

            //virtual bool colide_jogador(const Sphere& s) const = 0;
            virtual bool colide_jogador(const AABB& s) const = 0;
            virtual void desenha_poligono(int cor) = 0;
            virtual ~Poligono() = default;
    };

    class Cubo : public Poligono{
        public:
            float lado;

            Cubo(float ix, float iy, float iz, float l)
            : Poligono(ix,iy,iz,F::CUBO), lado(l) { }

            bool colide_jogador(const AABB& s) const override {
                AABB box = {{pos.x - lado, pos.y - lado, pos.z - lado}, {pos.x + lado, pos.y + lado, pos.z + lado}};
                return AABBvsAABB(s, box);//SphereVsAABB(s,box);
            }

            void desenha_poligono(int cor) override {
                if(cor >= 0 and cor <= 12) muda_cor(cor);
                glPushMatrix();
                glTranslatef(this->pos.x, this->pos.y, this->pos.z);
                // aqui `lado` é tratado como meio-extent (compatível com sua desenha_cubo)
                ND::desenha_cubo(this->lado);
                glPopMatrix();
            }

    };

    class Piramide : public Poligono{
        public:
            float base, altura;

            Piramide(float ix, float iy, float iz, float b, float h)
            : Poligono(ix,iy,iz,F::PIRAMIDE), base(b), altura(h) {}

            bool colide_jogador(const AABB& s) const override {
                float b = base / 2.0f;
                float h = altura / 2.0f;
                AABB box = {{pos.x - b, pos.y - h, pos.z - b}, {pos.x + b, pos.y + h, pos.z + b}};
                return AABBvsAABB(s,box);//SphereVsAABB(s, box);
                // base quad vertices (conforme ND::desenha_piramide)
                /*XYZ A = { pos.x - b, pos.y - b, pos.z - b };
                XYZ B = { pos.x + b, pos.y - b, pos.z - b };
                XYZ C = { pos.x + b, pos.y - b, pos.z + b };
                XYZ D = { pos.x - b, pos.y - b, pos.z + b };

                // apex (conforme ND::desenha_piramide: y = h - b)
                XYZ Apex = { pos.x, pos.y + (altura - b), pos.z };

                // base split into two triangles (A,B,C) and (A,C,D)
                if (SphereVsTriangle(s, A, B, C)) return true;
                if (SphereVsTriangle(s, A, C, D)) return true;

                // four lateral triangles
                if (SphereVsTriangle(s, A, B, Apex)) return true;
                if (SphereVsTriangle(s, B, C, Apex)) return true;
                if (SphereVsTriangle(s, C, D, Apex)) return true;
                if (SphereVsTriangle(s, D, A, Apex)) return true;

                return false;*/
            }

            void desenha_poligono(int cor) override {
                if(cor >= 0 and cor <= 12) muda_cor(cor);
                glPushMatrix();
                glTranslatef(this->pos.x, this->pos.y, this->pos.z);
                ND::desenha_piramide(this->base, this->altura);
                glPopMatrix();
            }
    };

    class Esfera : public Poligono{
        public:
            float raio;

            Esfera(float ix, float iy, float iz, float r)
            : Poligono(ix,iy,iz,F::ESFERA), raio(r) { }


            bool colide_jogador(const AABB& s) const override {
                AABB box = {{pos.x - raio, pos.y - raio, pos.z - raio}, {pos.x + raio, pos.y + raio, pos.z + raio}};
                return AABBvsAABB(s,box);//SphereVsAABB(s, box);
                //Sphere s2 = {{pos.x, pos.y, pos.z}, raio };
                //return SphereVsSphere(s, s2);
            }

            void desenha_poligono(int cor) override {
                if(cor >= 0 and cor <= 12) muda_cor(cor);
                glPushMatrix();
                glTranslatef(this->pos.x, this->pos.y, this->pos.z);
                ND::desenha_esfera(this->raio, 30, 30);
                glPopMatrix();
            }

    };

    class Cilindro : public Poligono{
        public:
            float raio, altura;
            XYZ centro_base, axis;

            Cilindro(float ix, float iy, float iz, float r, float h)
            : Poligono(ix,iy,iz,F::CILINDRO), raio(r), altura(h) 
             { centro_base = { this->pos.x, this->pos.y, this->pos.z - altura/2.0f };
                axis = { 0.0f, 0.0f, 1.0f }; }

            bool colide_jogador(const AABB& s) const override {
                AABB box = {
                    { pos.x - raio, pos.y - raio, pos.z - altura/2.0f },
                    { pos.x + raio, pos.y + raio, pos.z + altura/2.0f }
                };
                return AABBvsAABB(s,box);//SphereVsAABB(s, box);
                /*Cylinder cyl;
                cyl.base = centro_base;    // use o centro_base calculado no construtor
                // garante axis unitário (aqui já é eixo Z, mas normalizar é seguro)
                float len = sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
                cyl.axis = { axis.x / (len ? len : 1.0f), axis.y / (len ? len : 1.0f), axis.z / (len ? len : 1.0f) };
                cyl.h = altura;
                cyl.R = raio;
                return SphereVsCylinder(s, cyl);*/
            }

            void desenha_poligono(int cor) override {
                if(cor >= 0 and cor <= 12) muda_cor(cor);
                glPushMatrix();
                glTranslatef(this->pos.x, this->pos.y, this->pos.z);
                ND::desenha_cilindro(this->raio, this->altura, 30, 30, true);
                glPopMatrix();
            }
    };

    class Cone : public Poligono{
        public:
            float raio, altura;
            XYZ apex, axis;

            Cone(float ix, float iy, float iz, float r, float h)
            : Poligono(ix,iy,iz,F::CONE), raio(r), altura(h) 
            { apex = { this->pos.x, this->pos.y, this->pos.z + altura/2.0f };
                axis = { 0.0f, 0.0f, -1.0f }; }

            bool colide_jogador(const AABB& s) const override {
                AABB box = {
                    { pos.x - raio, pos.y - raio, pos.z - altura/2.0f },
                    { pos.x + raio, pos.y + raio, pos.z + altura/2.0f }
                };
                return AABBvsAABB(s,box);//SphereVsAABB(s, box);
                /*ConeBound cone;
                cone.apex = {pos.x, pos.y, pos.z + altura/2};  // ápice no topo
                cone.axis = {0,0,-1};  // apontando para baixo
                cone.h = altura;
                cone.R = raio;
                // Testa se o centro da esfera está dentro do cone expandido pelo raio da esfera
                return SphereVsCone(s,cone); //PointInConeBound(s.c, cone);*/
            }

            void desenha_poligono(int cor) override {
                if(cor >= 0 and cor <= 12) muda_cor(cor);
                glPushMatrix();
                glTranslatef(this->pos.x, this->pos.y, this->pos.z);
                ND::desenha_cone(this->raio, this->altura, 30);
                glPopMatrix();
            }
    };
}

vector<unique_ptr<NP::Poligono>> poligonos;

namespace NJ{ // NJ = Namespace para o Jogador

    using namespace NL;
    using namespace NB;
    using namespace NE;
    using namespace ND;
    using namespace NA;

    class Jogador : public Entidade{
        public:
            float cam_yaw, cam_pitch;
            AABB mascara; //Sphere mascara;

            Jogador(float ix, float iy, float iz, float cy, float cp)
            : Entidade(ix,iy,iz), cam_yaw(cy), cam_pitch(cp),
                mascara({{this->pos.x-1.0f,this->pos.y-1.0f,this->pos.z-1.0f},
                        {this->pos.x+1.0f,this->pos.y+1.0f,this->pos.z+1.0f}}) { }
            
            Jogador(){};

            void desenha_mascara(int stacks = 30, int fatias = 30){
                muda_cor(12);

                glBegin(GL_LINE_LOOP);
                glVertex3f(mascara.min.x,mascara.min.y,mascara.min.z);
                glVertex3f(mascara.min.x,mascara.max.y,mascara.min.z);
                glVertex3f(mascara.max.x,mascara.max.y,mascara.min.z);
                glVertex3f(mascara.max.x,mascara.min.y,mascara.min.z);
                glEnd();

                glBegin(GL_LINE_LOOP);
                glVertex3f(mascara.min.x,mascara.min.y,mascara.max.z);
                glVertex3f(mascara.min.x,mascara.max.y,mascara.max.z);
                glVertex3f(mascara.max.x,mascara.max.y,mascara.max.z);
                glVertex3f(mascara.max.x,mascara.min.y,mascara.max.z);
                glEnd();

                glBegin(GL_LINE_LOOP);
                glVertex3f(mascara.min.x,mascara.min.y,mascara.min.z);
                glVertex3f(mascara.min.x,mascara.min.y,mascara.max.z);
                glVertex3f(mascara.max.x,mascara.min.y,mascara.max.z);
                glVertex3f(mascara.max.x,mascara.min.y,mascara.min.z);
                glEnd();

                glBegin(GL_LINE_LOOP);
                glVertex3f(mascara.min.x,mascara.max.y,mascara.min.z);
                glVertex3f(mascara.min.x,mascara.max.y,mascara.max.z);
                glVertex3f(mascara.max.x,mascara.max.y,mascara.max.z);
                glVertex3f(mascara.max.x,mascara.max.y,mascara.min.z);
                glEnd();

                glBegin(GL_LINE_LOOP);
                glVertex3f(mascara.min.x,mascara.min.y,mascara.min.z);
                glVertex3f(mascara.min.x,mascara.max.y,mascara.min.z);
                glVertex3f(mascara.min.x,mascara.max.y,mascara.max.z);
                glVertex3f(mascara.min.x,mascara.min.y,mascara.max.z);
                glEnd();

                glBegin(GL_LINE_LOOP);
                glVertex3f(mascara.max.x,mascara.min.y,mascara.min.z);
                glVertex3f(mascara.max.x,mascara.max.y,mascara.min.z);
                glVertex3f(mascara.max.x,mascara.max.y,mascara.max.z);
                glVertex3f(mascara.max.x,mascara.min.y,mascara.max.z);
                glEnd();
                /*for (int i = 0; i < stacks; ++i) {
                    float phi1 = M_PI / 2 - i * (M_PI / stacks);
                    float phi2 = M_PI / 2 - (i + 1) * (M_PI / stacks);

                    glBegin(GL_LINE_LOOP);
                    for (int j = 0; j < fatias; ++j) {
                        float theta1 = j * (2 * M_PI / fatias);
                        float theta2 = (j + 1) * (2 * M_PI / fatias);

                        // Vertex 1 (bottom-left of current quad)
                        float x1 = this->mascara.r * cos(phi2) * sin(theta1) + this->pos.x;
                        float y1 = this->mascara.r * sin(phi2) + this->pos.y;
                        float z1 = this->mascara.r * cos(phi2) * cos(theta1) + this->pos.z;
                        glVertex3f(x1, y1, z1);

                        // Vertex 2 (bottom-right of current quad)
                        float x2 = this->mascara.r * cos(phi2) * sin(theta2) + this->pos.x;
                        float y2 = this->mascara.r * sin(phi2) + this->pos.y;
                        float z2 = this->mascara.r * cos(phi2) * cos(theta2) + this->pos.z;
                        glVertex3f(x2, y2, z2);

                        // Vertex 3 (top-right of current quad)
                        float x3 = this->mascara.r * cos(phi1) * sin(theta2) + this->pos.x;
                        float y3 = this->mascara.r * sin(phi1) + this->pos.y;
                        float z3 = this->mascara.r * cos(phi1) * cos(theta2) + this->pos.z;
                        glVertex3f(x3, y3, z3);

                        // Vertex 4 (top-left of current quad)
                        float x4 = this->mascara.r * cos(phi1) * sin(theta1) + this->pos.x;
                        float y4 = this->mascara.r * sin(phi1) + this->pos.y;
                        float z4 = this->mascara.r * cos(phi1) * cos(theta1) + this->pos.z;
                        glVertex3f(x4, y4, z4);
                    }
                glEnd();
                }*/
            }

            void desenha_mira(){
                float radYaw   = cam_yaw   * M_PI / 180.0f;
                float radPitch = cam_pitch * M_PI / 180.0f;
                float dirX = -sin(radYaw) * cos(radPitch);
                float dirY =  sin(radPitch);
                float dirZ = -cos(radYaw) * cos(radPitch);

                muda_cor(0);
                //glLineWidth(5.0f);
                glBegin(GL_LINES);
                glVertex3f(this->pos.x,this->pos.y,this->pos.z);
                glVertex3f(this->pos.x+dirX*1000.0f,this->pos.y+dirY*1000.0f,this->pos.z+dirZ*1000.0f);
                glEnd();
                //glLineWidth(1.0f);
            }

            bool detecta_adesivo(const Adesivo& a){
                float radYaw   = cam_yaw   * M_PI / 180.0f;
                float radPitch = cam_pitch * M_PI / 180.0f;
                float dirX = -sin(radYaw) * cos(radPitch);
                float dirY =  sin(radPitch);
                float dirZ = -cos(radYaw) * cos(radPitch);
                XYZ vi = {this->pos.x,this->pos.y,this->pos.z},
                    vf = {this->pos.x+dirX,this->pos.y+dirY,this->pos.z+dirZ},
                    va = {a.getX(),a.getY(),a.getZ()};
                float grau = Arccos((vf-vi),(va-vi)) * 180.0f / M_PI;
                return grau <= 20.0f and distancia_entidades(*this,a) <= 30.0f;
            }

            void tirou_foto(const Adesivo& a){
                if (flash_ativo and detecta_adesivo(a)) {
                    glDisable(GL_DEPTH_TEST);
                    glMatrixMode(GL_PROJECTION);
                    glPushMatrix();
                    glLoadIdentity();
                    glOrtho(0, 800, 600, 0, -1, 1);

                    glMatrixMode(GL_MODELVIEW);
                    glPushMatrix();
                    glLoadIdentity();

                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glColor4f(1.0f, 1.0f, 1.0f, flash_alpha);

                    glBegin(GL_QUADS);
                        glVertex2f(0, 0);
                        glVertex2f(800, 0);
                        glVertex2f(800, 600);
                        glVertex2f(0, 600);
                    glEnd();

                    glDisable(GL_BLEND);

                    glPopMatrix();
                    glMatrixMode(GL_PROJECTION);
                    glPopMatrix();
                    glMatrixMode(GL_MODELVIEW);
                    glEnable(GL_DEPTH_TEST);
                }

                if (flash_ativo) {
                    flash_alpha -= 2.0f * dt; // fade rápido (ajuste velocidade)
                    if (flash_alpha <= 0.0f) {
                        flash_alpha = 0.0f;
                        flash_ativo = false;
                    }
                }
            }

            bool tenta_mover(float dx, float dy, float dz){
                AABB candidate = this->mascara; //Sphere candidate = this->mascara;
                candidate.min.x += dx; candidate.max.x += dx;
                candidate.min.y += dy; candidate.max.y += dy;
                candidate.min.z += dz; candidate.max.z += dz;
                //candidate.c.x += dx;
                //candidate.c.y += dy;
                //candidate.c.z += dz;

                // testa contra todos os poligonos (use referências para evitar cópia)
                for (const auto& p : poligonos) 
                    if (p->colide_jogador(candidate)) 
                        return false; // colisão detectada => rejeita movimento

                // sem colisão => confirma movimento
                this->pos.x += dx;
                this->pos.y += dy;
                this->pos.z += dz;
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
                    float dx = - sin(rad) * dist * dt;
                    float dz = - cos(rad) * dist * dt;
                    tenta_mover(dx,0.0f,dz);
                } else {
                    float dy = dist * val * dt;
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
                    state = SDL_GetKeyboardState(NULL);
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
                this->mascara = {{this->pos.x-1.0f,this->pos.y-1.0f,this->pos.z-1.0f},
                        {this->pos.x+1.0f,this->pos.y+1.0f,this->pos.z+1.0f}};
                //this->mascara = {{this->pos.x,this->pos.y,this->pos.z},1.0f};
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
        "leftshoulder:b6,rightshoulder:b7,leftstick:b10,rightstick:b11,"
        "lefttrigger:b4,righttrigger:b5,"
        "dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,"
        "leftx:a0,lefty:a1,rightx:a2,righty:a3,"
    );

    for(int i = 0; i < SDL_NumJoysticks(); i++){
        if(SDL_IsGameController(i)){
            game_controller = SDL_GameControllerOpen(i);
            //modo_controle = "CONT";
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
    poligonos.push_back(make_unique<NP::Piramide>(10.0f,10.0f,-20.0f,4.0f,4.0f));
    poligonos.push_back(make_unique<NP::Esfera>(20.0f,10.0f,-20.0f,2.0f));
    poligonos.push_back(make_unique<NP::Cilindro>(30.0f,10.0f,-20.0f,2.0f,4.0f));
    poligonos.push_back(make_unique<NP::Cone>(40.0f,10.0f,-20.0f,2.0f,4.0f));
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
            
            if(!game_controller){
                if(evento.type == SDL_KEYDOWN){
                    if(evento.key.keysym.sym == SDLK_p){
                        pause = !pause;
                        SDL_ShowCursor(pause ? SDL_ENABLE : SDL_DISABLE);
                    } else if(evento.key.keysym.sym == SDLK_ESCAPE)
                        if(pause) rodando = false;
                        else primeira_pessoa = !primeira_pessoa;
                }
                if(evento.type == SDL_MOUSEBUTTONDOWN and pause){
                    pause = false;
                    SDL_ShowCursor(SDL_DISABLE);
                } else if(evento.type == SDL_MOUSEBUTTONDOWN and !pause){
                    if(evento.button.button == SDL_BUTTON_LEFT){
                        flash_alpha = 1.0f;
                        flash_ativo = true;
                    }
                }
            } else {
                if(evento.type == SDL_CONTROLLERBUTTONDOWN) {
                    if(evento.cbutton.button == SDL_CONTROLLER_BUTTON_START)
                        pause = !pause;
                    else if(evento.cbutton.button == SDL_CONTROLLER_BUTTON_BACK)
                        if(pause) rodando = false;
                        else primeira_pessoa = !primeira_pessoa;
                }
                if(SDL_GameControllerGetAxis(game_controller,SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 16000){
                    flash_alpha = 1.0f;
                    flash_ativo = true;
                }
            }
        }

        // -------------------- detectar trigger do controle (por frame, com rising edge) --------------------
        if (game_controller) {
            Sint16 rt = SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
            bool rtPressed = (rt > 16000); // threshold; se seu controlador usar signed axis com repouso negativo, ajuste abaixo

            static bool prevRTPressed = false; // preserva estado entre frames
            if (rtPressed && !prevRTPressed) {
                flash_alpha = 1.0f;
                flash_ativo = true;
                // não chamar jogador.tirou_foto aqui — desenhe no render loop
            }
            prevRTPressed = rtPressed;
        }

        // Limpa tela
        if(!game_controller and !pause) glClearColor(1.0f,0.0f,0.5f,1.0f);
        else if(!game_controller and pause) glClearColor(0.5f,0.0f,0.25f,1.0f);
        else if(!pause) glClearColor(0.5f,0.0f,0.5f,1.0f);
        else glClearColor(0.25f,0.0f,0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        if(primeira_pessoa) {
            glRotatef(-jogador.cam_pitch, 1.0, 0.0, 0.0); 
            glRotatef(-jogador.cam_yaw, 0.0, 1.0, 0.0);
            glTranslatef(-(jogador.getX()),-(jogador.getY()),-(jogador.getZ()));        
        } else {
            gluLookAt(jogador.getX(),jogador.getY()+5.0f,jogador.getZ()+25.0f,
                jogador.getX(),jogador.getY(),jogador.getZ(),
                0.0f,1.0f,0.0f);
            jogador.desenha_mascara();
        }
        jogador.desenha_mira();

        // Desenha chão
		glPushMatrix();
			glTranslatef(0,-1,0);
			glScalef(100,-0.1,100);
        	ND::desenha_chao();
		glPopMatrix();

        for(int i = 0; i < 26; i+=2){
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
        }

        for (const auto& p : poligonos)
            p->desenha_poligono(1);

        a.desenha_adesivo();

        // Controla câmera
        jogador.controle_camera(MOVE_VEL, CAMERA_SENS);
        if(jogador.detecta_adesivo(a)){
            glDisable(GL_DEPTH_TEST);   // ignora profundidade
            ND::marcax(a.getX(),a.getY(),a.getZ());
            glEnable(GL_DEPTH_TEST);    // reativa para os próximos frames
        }

        jogador.tirou_foto(a);

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

    cria_poligonos(5);

    loop_jogo();

    finaliza_sdl();

    return 0;
}