#include "math.h"
#include "shape.h"

class Light {
public:
    virtual vec3 Sample_Li(const vec3 &pos, const vec2 &u, vec3 *wi, float *pdf, float *t) const = 0;
    
    virtual float Pdf_Li(const vec3 &pos, const vec3 &wi, const RTCScene &scene, const unsigned int &diskID) const = 0;

    virtual vec3 L() const = 0;

// protected:
//     const mat3 LightToWorld, WorldToLight;
};

class PointLight: public Light {
public:
    PointLight(const vec3 &_pLight, const vec3 &_I): pLight(_pLight), I(_I) {}

    vec3 Sample_Li(const vec3 &pos, const vec2 &u, vec3 *wi, float *pdf, float *t) const {
        *wi = pLight - pos;
        *wi = normalize(*wi);
        *pdf = 1.f;
        return I / (pLight - pos).lengthSquared();
    }

    float Pdf_Li(const vec3 &pos, const vec3 &wi, const RTCScene &scene, const unsigned int &diskID) const {
        return 0; // infinitesimal light source
    }

private:
    const vec3 pLight;
    const vec3 I; // intensity, the amount of power per unit solid angle
};

class DiffuseAreaLight: public Light {
public:
    DiffuseAreaLight(Disk *_disk, const vec3 &_Lemit): disk(_disk), Lemit(_Lemit) {}

    vec3 Sample_Li(const vec3 &pos, const vec2 &u, vec3 *wi, float *pdf, float *t) const {
        vec3 sampledPos = disk->Sample(u); // world space
        vec3 dist = sampledPos - pos;
        *wi = normalize(dist);
        *t = dist.y / (*wi).y; // todo: check devided by zero
        *pdf = disk->Pdf();

        return Lemit;
    }

    float Pdf_Li(const vec3 &pos, const vec3 &wi, const RTCScene &scene, const unsigned int &diskID) const {
        return disk->Pdf(pos, wi, scene, diskID);
    }

    vec3 L() const { // todo: check wi and normal in same hemisphere
        return Lemit;
    }

private:
    Disk *disk;
    const vec3 Lemit;
};