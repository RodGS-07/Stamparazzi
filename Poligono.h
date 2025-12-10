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
        float escalax = 1.0f, escalay = 1.0f, escalaz = 1.0f, rotx = 0.0f, roty = 0.0f, rotz = 0.0f;
        unique_ptr<Adesivo> adesivo;

    public:
        Poligono(int s);
        Poligono(float ix, float iy, float iz, int s, unique_ptr<Adesivo> a);
        Poligono(float ix, float iy, float iz, int op, float xs, float ys, float zs, int s, unique_ptr<Adesivo> a);
        //Poligono(float ix, float iy, float iz, float xs, float ys, float zs, int s, unique_ptr<Adesivo> a);

        //virtual bool colide_jogador(const Sphere& s) const = 0;
        int getSuperficie() const;
        float getEscalaX() const;
        float getEscalaY() const;
        float getEscalaZ() const;
        float getRotX() const;
        float getRotY() const;
        float getRotZ() const;
        void setEscala(float xs, float ys, float zs);
        Adesivo* getAdesivo() const;
        void setAdesivo(unique_ptr<Adesivo> a);
        void desenha_adesivo_no_poligono(const Adesivo& adesivo, float offset);
        virtual AABB getAABB() const = 0;
        virtual void realiza_movimento(float dt, bool pause, bool modo_daltonico)= 0;
        virtual bool colide_jogador(const AABB& s) const = 0;
        virtual void aplica_efeito(Jogador& jogador, int& vidas) = 0;
        virtual void desenha_poligono(int cor, bool pause, bool modo_daltonico) = 0;
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
        Cubo(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float l);

        AABB getAABB() const override;
        float getLado() const;
        void realiza_movimento(float dt, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_poligono(int cor, bool pause, bool modo_daltonico) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
};

class Piramide : public Poligono{
    private:
        float base, altura;

    public:
        Piramide();
        Piramide(float ix, float iy, float iz, unique_ptr<Adesivo> a, float b, float h);
        Piramide(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float b, float h);

        AABB getAABB() const override;
        float getAltura() const;
        void realiza_movimento(float dt, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_poligono(int cor, bool pause, bool modo_daltonico) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
};

class Esfera : public Poligono{
    private:
        float raio, y_vel, grav, chao, altura_inicial, velocidade_inicial;

    public:
        Esfera();
        Esfera(float ix, float iy, float iz, unique_ptr<Adesivo> a, float r, float c);
        Esfera(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float r, float c);

        AABB getAABB() const override;
        float getRaio() const;
        void realiza_movimento(float dt, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_poligono(int cor, bool pause, bool modo_daltonico) override;
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
        Cilindro(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float r, float h);

        AABB getAABB() const override;
        float getRaio() const;
        float getAltura() const;
        void realiza_movimento(float dt, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_poligono(int cor, bool pause, bool modo_daltonico) override;
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
        Cone(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float r, float h);

        AABB getAABB() const override;
        float getRaio() const;
        float getAltura() const;
        void realiza_movimento(float dt, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_poligono(int cor, bool pause, bool modo_daltonico) override;
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
        Torus(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float re, float ra);

        Torus* getConjugado() const;
        void setConjugado(Torus* t);
        AABB getAABB() const override;
        void realiza_movimento(float dt, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_poligono(int cor, bool pause, bool modo_daltonico) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
};

#endif