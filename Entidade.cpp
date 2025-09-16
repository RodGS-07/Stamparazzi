// Código para todas as propriedades comuns a todas as Entidades
#include "Entidade.h"
#include "Linear.h"
#include <cmath>

// Construtores
Entidade::Entidade() : pos({0.0f,0.0f,0.0f}) {}
Entidade::Entidade(float ix, float iy, float iz) {
    pos.x = ix; pos.y = iy; pos.z = iz;
}

// Métodos getters
float Entidade::getX() const { return pos.x; }
float Entidade::getY() const { return pos.y; }
float Entidade::getZ() const { return pos.z; }

// Métodos setters
void Entidade::setX(float ix) { pos.x = ix; }
void Entidade::setY(float iy) { pos.y = iy; }
void Entidade::setZ(float iz) { pos.z = iz; }

// Função livre
float distancia_entidades(const Entidade& e1, const Entidade& e2) {
    float dx = e1.getX() - e2.getX();
    float dy = e1.getY() - e2.getY();
    float dz = e1.getZ() - e2.getZ();
    return sqrt(dx*dx + dy*dy + dz*dz);
}