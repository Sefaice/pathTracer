#include "math.h"

inline vec2 uniformSampleDisk(const vec2 &u) {
    float r = sqrt(u.x);
    float theta = 2 * M_PI * u.y;
    return vec2(r * cos(theta), r * sin(theta));
}

class Disk {
public:
    Disk(float _radius, mat4 *_ObjectToWorld, mat4 *_WorldToObject): 
        radius(_radius), ObjectToWorld(_ObjectToWorld), WorldToObject(_WorldToObject) {}

    vec3 Sample(const vec2 &u) const {
        vec2 pd = uniformSampleDisk(u);
        vec4 pObj(pd.x * radius, pd.y * radius, 0, 1);
        vec4 pWorld = (*ObjectToWorld) * pObj;
        return vec3(pWorld.x, pWorld.y, pWorld.z);
    }

    float Area() const {
        return M_PI * radius * radius;
    }

    float Pdf() const {
        return 1 / Area();
    }

    float Pdf(const vec3 pos, const vec3 intersectPos, const vec3 intersectN, const vec3 &wi) const {
        // <<Intersect sample ray with area light geometry>> 
        // Ray ray = ref.SpawnRay(wi);
        // Float tHit;
        // SurfaceInteraction isectLight;
        // if (!Intersect(ray, &tHit, &isectLight, false)) return 0;

        // <<Convert light sample weight to solid angle measure>> 
        float pdf = (pos - intersectPos).lengthSquared() /
                    std::abs(dot(intersectN, -wi) * Area());
        
        return pdf;
    }

private:
    const float radius;
    const mat4 *ObjectToWorld, *WorldToObject;
};