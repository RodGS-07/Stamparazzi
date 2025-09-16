#ifndef ENTIDADE_H
#define ENTIDADE_H

#include "Linear.h"
#include <cmath>

class Entidade {
    private:
        XYZ pos;   // posição (x,y,z)

    public:
        // Construtores
        Entidade();
        Entidade(float ix, float iy, float iz);

        // Gets
        float getX() const;
        float getY() const;
        float getZ() const;

        // Sets
        void setX(float ix);
        void setY(float iy);
        void setZ(float iz);
    };

    // Função auxiliar (pode ficar no header como inline, ou em Entidade.cpp)
    float distancia_entidades(const Entidade& e1, const Entidade& e2);

#endif