#ifndef ADESIVO_H
#define ADESIVO_H

#include "Entidade.h"
#include "Draw.h"

class Adesivo : public Entidade{
    private:
        int texturaID;
        XYZ normal;

    public:
        Adesivo();
        Adesivo(float ix, float iy, float iz, int t, XYZ n);

        bool operator<(const Adesivo& outro) const;

        int getTexturaID() const;

        void setTexturaID(int t);

        XYZ getNormal() const;

        void setNormal(XYZ n);

        void desenha_adesivo() const;
};


#endif