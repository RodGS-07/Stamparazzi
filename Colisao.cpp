// Código para Bounding Boxes e Colisões
#include "Colisao.h"
#include "Linear.h"
#include <algorithm>

#define XBOUNDS 100.0f
#define YBOUNDS 100.0f
#define ZBOUNDS 100.0f

using namespace std;

struct AABB {
    XYZ min, max;
};

struct Sphere {
    XYZ c;
    float r;
};

struct Portal{
    XYZ c;
    float raio_menor, raio_maior;
    Portal* par;
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

bool SegmentVsAABB(const XYZ& p0, const XYZ& p1, const AABB& box) {
    XYZ dir = p1 - p0;
    float tmin = 0.0f;
    float tmax = 1.0f;

    for (int i = 0; i < 3; i++) {
        float invD = 1.0f / (i==0 ? dir.x : (i==1 ? dir.y : dir.z));
        float t0 = ((i==0 ? box.min.x : (i==1 ? box.min.y : box.min.z)) - 
                    (i==0 ? p0.x : (i==1 ? p0.y : p0.z))) * invD;
        float t1 = ((i==0 ? box.max.x : (i==1 ? box.max.y : box.max.z)) - 
                    (i==0 ? p0.x : (i==1 ? p0.y : p0.z))) * invD;
        if (invD < 0.0f) swap(t0, t1);

        tmin = max(tmin, t0);
        tmax = min(tmax, t1);

        if (tmax < tmin) return false;
    }

    return true;
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