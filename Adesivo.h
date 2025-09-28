#ifndef ADESIVO_H
#define ADESIVO_H

#include "Entidade.h"
#include "Draw.h"

class Adesivo : public Entidade{
    private:
        XYZ normal;

    public:
        Adesivo();
        Adesivo(float ix, float iy, float iz, XYZ n);

        XYZ getNormal() const;

        void setNormal(XYZ n);

        void desenha_adesivo() const;
};


#endif