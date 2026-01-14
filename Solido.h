#ifndef SOLIDO_H
#define SOLIDO_H

#include "Entidade.h"
#include "Draw.h"
#include "Colisao.h"
#include "Adesivo.h"
#include <SDL2/SDL_mixer.h>
#include <memory>

using namespace std;

class Jogador;
class Adesivo;

class Solido : public Entidade{
    private:
        int superficie;
        float escalax = 1.0f, escalay = 1.0f, escalaz = 1.0f, rotx = 0.0f, roty = 0.0f, rotz = 0.0f;
        unique_ptr<Adesivo> adesivo;

    public:
        Solido(int s);
        Solido(float ix, float iy, float iz, int s, unique_ptr<Adesivo> a);
        Solido(float ix, float iy, float iz, int op, float xs, float ys, float zs, int s, unique_ptr<Adesivo> a);
        //Solido(float ix, float iy, float iz, float xs, float ys, float zs, int s, unique_ptr<Adesivo> a);

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
        void desenha_adesivo_no_solido(const Adesivo& adesivo, float offset);
        virtual AABB getAABB() const = 0;
        virtual void realiza_movimento(float dt, float dist, bool pause, bool modo_daltonico)= 0;
        virtual bool colide_jogador(const AABB& s) const = 0;
        virtual void aplica_efeito(Jogador& jogador, int& vidas) = 0;
        virtual void desenha_solido(int cor, bool pause, bool modo_daltonico) = 0;
        //virtual void desenha_adesivo() = 0;
        virtual void desenha_mascara() = 0;
        virtual void finaliza_som() = 0;
        virtual ~Solido() = default;
};

class Cubo : public Solido{
    private:
        float lado;

    public:
        Cubo();
        Cubo(float ix, float iy, float iz, unique_ptr<Adesivo> a, float l);
        Cubo(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float l);

        AABB getAABB() const override;
        float getLado() const;
        void realiza_movimento(float dt, float dist, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_solido(int cor, bool pause, bool modo_daltonico) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
        void finaliza_som();
};

class Piramide : public Solido{
    private:
        float base, altura;

    public:
        Piramide();
        Piramide(float ix, float iy, float iz, unique_ptr<Adesivo> a, float b, float h);
        Piramide(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float b, float h);

        AABB getAABB() const override;
        float getAltura() const;
        void realiza_movimento(float dt, float dist, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_solido(int cor, bool pause, bool modo_daltonico) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
        void finaliza_som();
};

class Esfera : public Solido{
    private:
        float raio, y_vel, grav, chao, altura_inicial, velocidade_inicial;
        Mix_Chunk* som_batida = Mix_LoadWAV("Audio/Efeitos_Sonoros/mixkit-bola-quicando.wav");

    public:
        Esfera();
        Esfera(float ix, float iy, float iz, unique_ptr<Adesivo> a, float r, float c);
        Esfera(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float r, float c);

        AABB getAABB() const override;
        float getRaio() const;
        void realiza_movimento(float dt, float dist, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_solido(int cor, bool pause, bool modo_daltonico) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
        void finaliza_som();
};

class Cilindro : public Solido{
    private:
        float raio, altura, x_vel, ang;
        XYZ centro_base, axis;
        int canal_rolo = -1;
        Mix_Chunk* som_rolo = Mix_LoadWAV("Audio/Efeitos_Sonoros/mixkit-cilindro-rolando.wav");

    public:
        Cilindro();
        Cilindro(float ix, float iy, float iz, unique_ptr<Adesivo> a, float r, float h);
        Cilindro(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float r, float h);

        AABB getAABB() const override;
        float getRaio() const;
        float getAltura() const;
        void realiza_movimento(float dt, float dist, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_solido(int cor, bool pause, bool modo_daltonico) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
        void finaliza_som();
};

class Cone : public Solido{
    private:
        float raio, altura, ang;
        XYZ apex, axis;
        int canal_laser = -1;
        Mix_Chunk* som_laser = Mix_LoadWAV("Audio/Efeitos_Sonoros/mixkit-estatica-laser.wav");

    public:
        Cone();
        Cone(float ix, float iy, float iz, unique_ptr<Adesivo> a, float r, float h);
        Cone(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float r, float h);

        AABB getAABB() const override;
        float getRaio() const;
        float getAltura() const;
        void realiza_movimento(float dt, float dist, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_solido(int cor, bool pause, bool modo_daltonico) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
        void finaliza_som();
};

class Torus : public Solido{
    private:
        Portal p;
        Torus* conjugado;
        Mix_Chunk* som_teleporte = Mix_LoadWAV("Audio/Efeitos_Sonoros/mixkit-teleporte.wav");

    public:
        Torus();
        Torus(float ix, float iy, float iz, unique_ptr<Adesivo> a, float re, float ra);
        Torus(float ix, float iy, float iz, int op, float xs, float ys, float zs, unique_ptr<Adesivo> a, float re, float ra);

        Torus* getConjugado() const;
        void setConjugado(Torus* t);
        AABB getAABB() const override;
        void realiza_movimento(float dt, float dist, bool pause, bool modo_daltonico) override;
        bool colide_jogador(const AABB& s) const override;
        void aplica_efeito(Jogador& jogador, int& vidas) override;
        void desenha_solido(int cor, bool pause, bool modo_daltonico) override;
        //void desenha_adesivo() override;
        void desenha_mascara();
        void finaliza_som();
};

#endif