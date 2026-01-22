#ifndef LINEAR_H
#define LINEAR_H

#include <GL/glut.h>

// Representa pontos e vetores
struct XYZ {
    float x, y, z;
    XYZ();
    XYZ(float ix, float iy, float iz);
};

// Norma
GLfloat operator!(const XYZ& v);

// Diferença de vetores
XYZ operator-(const XYZ& o, const XYZ& d);

// Vetor oposto
XYZ operator-(const XYZ& v);

// Soma de vetores
XYZ operator+(const XYZ& o, const XYZ& d);

// Produto escalar
GLfloat Escalar(const XYZ& u, const XYZ& v);

// Produto vetorial
XYZ operator*(const XYZ& u, const XYZ& v);

// Produto de vetor por escalar
XYZ operator*(const XYZ& u, GLfloat lu);

// Divisão de vetor por escalar
XYZ operator/(const XYZ& u, GLfloat lu);

// Normal a partir de 3 pontos
XYZ Normal(XYZ Pa, XYZ Pb, XYZ Pc);

// Ângulo (arccos) entre dois vetores
GLfloat Arccos(XYZ Pa, XYZ Pb);

// Vetor rotacionado no eixo x
XYZ rotX(const XYZ& v, float ang);

// Vetor rotacionado no eixo y
XYZ rotY(const XYZ& v, float ang);

// Vetor rotacionado no eixo z
XYZ rotZ(const XYZ& v, float ang);

#endif