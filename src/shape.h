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
        vec4 pObj(pd.x, pd.y, 0, 1);
        vec4 pWorld = (*ObjectToWorld) * pObj;

        return vec3(pWorld.x, pWorld.y, pWorld.z);
    }

    float Area() const {
        return M_PI * radius * radius;
    }

    float Pdf() const {
        return 1 / Area();
    }

    float Pdf(const vec3 &pos, const vec3 &wi, const RTCScene &scene, const unsigned int &diskID) const {
        // Intersect sample ray with area light geometry, scene ONLY contains the disk
        struct RTCIntersectContext context;
        rtcInitIntersectContext(&context);
        struct RTCRayHit rayhit;
        rayhit.ray.org_x = pos.x; rayhit.ray.org_y = pos.y; rayhit.ray.org_z = pos.z;
        rayhit.ray.dir_x = wi.x; rayhit.ray.dir_y = wi.y; rayhit.ray.dir_z = wi.z;
        rayhit.ray.tnear = 0; rayhit.ray.tfar = INF;
        rayhit.ray.mask = -1; rayhit.ray.flags = 0;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
        rtcIntersect1(scene, &context, &rayhit);
        if (rayhit.hit.geomID != diskID) return 0; // no hit
        
        vec3 intersectN = vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z);
        intersectN = normalize(intersectN);
        vec3 intersectPos = pos + wi * rayhit.ray.tfar;

        // Convert light sample weight to solid angle measure
        float pdf = (pos - intersectPos).lengthSquared() /
                    std::abs(dot(intersectN, -wi) * Area());
        
        return pdf;
    }

private:
    const float radius;
    const mat4 *ObjectToWorld, *WorldToObject;
};