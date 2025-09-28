#ifndef POLIGONO_H
#define POLIGONO_H

#include "Entidade.h"
#include "Draw.h"
#include "Colisao.h"
#include "Adesivo.h"
#include <memory>

using namespace std;

class Jogador;
class Adesivo;

class Poligono : public Entidade{
    private:
        int superficie;
        unique_ptr<Adesivo> adesivo;

    public:
        Poligono(int s);
        Poligono(float ix, float iy, float iz, int s, unique_ptr<Adesivo> a);

        //virtual bool colide_jogador(const Sphere& s) const = 0;
        int getSuperficie() const;
        Adesivo* getAdesivo() const;
        void setAdesivo(unique_ptr<Adesivo> a);
        void desenha_adesivo_no_poligono(const Adesivo& adesivo, float offset);
        virtual AABB getAABB() const = 0;
        virtual void realiza_movimento(int cor, float dt, bool pause)= 0;
        virtual bool colide_jogador(const AABB& s) const = 0;
        virtual void aplica_efeito(Jogador& jogador) = 0;
        virtual void desenha_poligono(int cor, bool pause) = 0;
        //virtual void desenha_adesivo() = 0;
        virtual void desenha_mascara() = 0;
        virtual ~Poligono() = default;
};

class Cubo : public Poligono{
    private:
        float lado;

    public:
        Cubo();
        Cubo(float ix, float iy, float iz, unique_ptr<Adesivo> a, float l);

        AABB getAABB() const override;
        float getLado() const;
        void realiza_movimento(int cor, float dt, bool pause) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador) override;
        void desenha_poligono(int cor, bool pause) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
};

class Piramide : public Poligono{
    private:
        float base, altura;

    public:
        Piramide();
        Piramide(float ix, float iy, float iz, unique_ptr<Adesivo> a, float b, float h);

        AABB getAABB() const override;
        float getAltura() const;
        void realiza_movimento(int cor, float dt, bool pause) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador) override;
        void desenha_poligono(int cor, bool pause) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
};

class Esfera : public Poligono{
    private:
        float raio, y_vel, grav, chao;

    public:
        Esfera();
        Esfera(float ix, float iy, float iz, unique_ptr<Adesivo> a, float r);

        AABB getAABB() const override;
        float getRaio() const;
        void realiza_movimento(int cor, float dt, bool pause) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador) override;
        void desenha_poligono(int cor, bool pause) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
};

class Cilindro : public Poligono{
    private:
        float raio, altura, x_vel, ang;
        XYZ centro_base, axis;

    public:
        Cilindro();
        Cilindro(float ix, float iy, float iz, unique_ptr<Adesivo> a, float r, float h);

        AABB getAABB() const override;
        float getRaio() const;
        float getAltura() const;
        void realiza_movimento(int cor, float dt, bool pause) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador) override;
        void desenha_poligono(int cor, bool pause) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
};

class Cone : public Poligono{
    private:
        float raio, altura, ang;
        XYZ apex, axis;

    public:
        Cone();
        Cone(float ix, float iy, float iz, unique_ptr<Adesivo> a, float r, float h);

        AABB getAABB() const override;
        float getRaio() const;
        float getAltura() const;
        void realiza_movimento(int cor, float dt, bool pause) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador) override;
        void desenha_poligono(int cor, bool pause) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
};

class Torus : public Poligono{
    private:
        Portal p;
        Torus* conjugado;

    public:
        Torus();
        Torus(float ix, float iy, float iz, unique_ptr<Adesivo> a, float re, float ra);

        Torus* getConjugado() const;
        void setConjugado(Torus* t);
        AABB getAABB() const override;
        void realiza_movimento(int cor, float dt, bool pause) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador) override;
        void desenha_poligono(int cor, bool pause) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
};

#endif