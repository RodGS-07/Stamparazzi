#ifndef COLISAO_H
#define COLISAO_H

#include "Linear.h"

struct AABB {
    XYZ min, max;
};

struct Sphere {
    XYZ c;
    float r;
};

struct Cylinder {
    XYZ base;   // centro da base
    XYZ axis;   // vetor normalizado do eixo
    float h;     // altura
    float R;     // raio
};

struct Capsule {
    XYZ A,B; // extremos do segmento central
    float r;  // raio
};

struct ConeBound {
    XYZ apex;   // vértice
    XYZ axis;   // direção (unitário, do ápice à base)
    float h;     // altura
    float R;     // raio da base
};

struct Portal{
    XYZ c;
    float raio_menor, raio_maior;
    Portal* par;
};

bool AABBvsAABB(const AABB& a, const AABB& b);
bool SphereVsSphere(const Sphere &a, const Sphere &b);
bool SphereVsAABB(const Sphere &s, const AABB &b);
bool SegmentVsAABB(const XYZ& p0, const XYZ& p1, const AABB& box);
inline float Length2(const XYZ&a);
inline float Clamp(float v,float mn,float mx);
XYZ ClosestPointOnSegment(const XYZ& A,const XYZ& B,const XYZ& P);
float SegmentSegmentDist2(const XYZ& A0,const XYZ& A1,
                        const XYZ& B0,const XYZ& B1,
                        float& s,float& t);
bool CapsuleVsCapsule(const Capsule& c1,const Capsule& c2);
bool SphereVsCylinder(const Sphere& s,const Cylinder& cyl);
bool PointInConeBound(const XYZ& P,const ConeBound& cone);
// -----------------------------------------------------------
// Closest point on triangle (Ericson) + Sphere vs Triangle
// -----------------------------------------------------------
XYZ ClosestPointOnTriangle(const XYZ& A, const XYZ& B, const XYZ& C, const XYZ& P);
bool SphereVsTriangle(const Sphere& s, const XYZ& A, const XYZ& B, const XYZ& C);
// -----------------------------------------------------------
// Sphere vs Cone bounded (approx / robust test)
// cone.axis must be normalized, cone.apex at apex, h = height, R = base radius
// -----------------------------------------------------------
bool SphereVsCone(const Sphere& s, const ConeBound& cone);
AABB unionAABB(const AABB &a, const AABB &b);
XYZ computeMTV_AABB_vs_AABB(const AABB &a, const AABB &b);

#endif