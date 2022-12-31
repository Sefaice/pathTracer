#include "math.h"
#include "shape.h"

class Light {
public:
    virtual vec3 Sample_Li(const vec3 &pos, const vec2 &u, vec3 *wi, float *pdf) const = 0;
    
    virtual float Pdf_Li(const vec3 pos, const vec3 intersectPos, const vec3 intersectN, const vec3 &wi) const = 0;

// protected:
//     const mat3 LightToWorld, WorldToLight;
};

class PointLight: public Light {
public:
    PointLight(const vec3 &_pLight, const vec3 &_I): pLight(_pLight), I(_I) {}

    vec3 Sample_Li(const vec3 &pos, const vec2 &u, vec3 *wi, float *pdf) const {
        *wi = pLight - pos;
        *wi = normalize(*wi);
        *pdf = 1.f;
        return I / (pLight - pos).lengthSquared();
    }

    float Pdf_Li(const vec3 pos, const vec3 intersectPos, const vec3 intersectN, const vec3 &wi) const {
        return 0; // infinitesimal light source
    }

private:
    const vec3 pLight;
    const vec3 I; // intensity, the amount of power per unit solid angle
};

class DiffuseAreaLight: public Light {
public:
    DiffuseAreaLight(Disk *_disk, const vec3 &_Lemit): disk(_disk), Lemit(_Lemit) {}

    vec3 Sample_Li(const vec3 &pos, const vec2 &u, vec3 *wi, float *pdf) const {
        vec3 sampledPos = disk->Sample(u); // world space
        *wi = sampledPos - pos;
        *wi = normalize(*wi);
        *pdf = disk->Pdf();

        return Lemit;
    }

    //float Pdf_Li(const Interaction &ref, const Vector3f &wi) const {
    float Pdf_Li(const vec3 pos, const vec3 intersectPos, const vec3 intersectN, const vec3 &wi) const {
        return disk->Pdf(pos, intersectPos, intersectN, wi);
    }

private:
    Disk *disk;
    const vec3 Lemit;
};