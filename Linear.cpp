// Código para operações de álgebra linear envolvendo vetores
#include "Linear.h"
#include <GL/glut.h>
#include <cmath>

XYZ::XYZ() : x(0), y(0), z(0) {}
XYZ::XYZ(float x, float y, float z) : x(x), y(y), z(z) {}

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
XYZ rotX(const XYZ& v, float ang) {//Vetor rotacionado no eixo x
    float c = cos(ang), s = sin(ang);
    return XYZ(
        v.x,
        v.y * c - v.z * s,
        v.y * s + v.z * c
    );
}
XYZ rotY(const XYZ& v, float ang) {//Vetor rotacionado no eixo y
    float c = cos(ang), s = sin(ang);
    return XYZ(
        v.x * c + v.z * s,
        v.y,
        -v.x * s + v.z * c
    );
}
XYZ rotZ(const XYZ& v, float ang) {//Vetor rotacionado no eixo z
    float c = cos(ang), s = sin(ang);
    return XYZ(
        v.x * c - v.y * s,
        v.x * s + v.y * c,
        v.z
    );
}