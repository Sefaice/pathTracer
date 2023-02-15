#include "math.h"

inline bool SameHemisphere(const vec3 &w, const vec3 &wp) {
    return w.z * wp.z > 0;
}

// from GAMES
inline mat3 localToWorld(const vec3 &N) {
	float x2z2 = N.x * N.x + N.z * N.z;
	vec3 T = vec3(N.x * N.y, -x2z2, N.z * N.y); // self-normalized
    T = normalize(T);
    if (T.length() == 0) // no need to transform
        return mat3(1);
	vec3 B = cross(N, T);
    B = normalize(B);
	mat3 TBN = mat3(T, B, N);

	return TBN;
}


inline vec3 cartesian(const float phi, const float sinTheta, const float cosTheta)
{
  const float sinPhi = sinf(phi);
  const float cosPhi = cosf(phi);
  //sincosf(phi, &sinPhi, &cosPhi);
  return vec3(cosPhi * sinTheta,
                    sinPhi * sinTheta,
                    cosTheta);
}

inline vec3 cartesian(const float phi, const float cosTheta)
{
  return cartesian(phi, sqrt(1 - cosTheta * cosTheta), cosTheta);
}

inline vec3 cosineSampleHemisphere(const vec2 s)
{
  const float phi = float(M_PI * 2) * s.x;
  const float cosTheta = sqrt(s.y);
  const float sinTheta = sqrt(1.0f - s.y);
  return cartesian(phi, sinTheta, cosTheta);
}

class BxDF {
public:
    virtual ~BxDF() {}
    //BxDF(BxDFType type) : type(type) { }
    // bool MatchesFlags(BxDFType t) const {
    //     return (type & t) == type;
    // }

    virtual vec3 f(const vec3 &wo, const vec3 &wi) const = 0;

    virtual vec3 Sample_f(const vec3 &wo, vec3 *wi, const vec2 &sample, float *pdf) const = 0;

    virtual vec3 rho(const vec3 &wo, int nSamples, const vec2 *samples) const = 0;
    virtual vec3 rho(int nSamples, const vec2 *samples1, const vec2 *samples2) const = 0;

    virtual float Pdf(const vec3 &wo, const vec3 &wi) const = 0;

    //const BxDFType type;
};

// class SpecularReflection: public BxDF {

//     vec3 f(const vec3 &wo, const vec3 &wi) const {
//         return vec3(0);
//     }

//     vec3 Sample_f(const vec3 &wo, vec3 *wi, const vec2 &sample, float *pdf) const {
//         *wi = vec3(-wo.x, -wo.y, wo.z);
//         *pdf = 1;
//         return fresnel->Evaluate(CosTheta(*wi)) * R / AbsCosTheta(*wi);
//     }

//     float Pdf(const vec3 &wo, const vec3 &wi) const {
//         return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * InvPi : 0;
//     }
// };

class LambertianReflection: public BxDF {
public:
    LambertianReflection(const vec3 &R): R(R) {}

    vec3 f(const vec3 &wo, const vec3 &wi) const {
        return R * INV_PI;
    }

    vec3 Sample_f(const vec3 &wo, vec3 *wi, const vec2 &sample, float *pdf) const {
        *wi = cosineSampleHemisphere(sample);
        *pdf = Pdf(wo, *wi);
        return f(wo, *wi);
    }

    vec3 rho(const vec3 &wo, int nSamples, const vec2 *samples) const { return R; }
    vec3 rho(int nSamples, const vec2 *samples1, const vec2 *samples2) const { return R; }

    inline float AbsCosTheta(const vec3 &w) const { 
        return std::abs(w.z); 
    }

    float Pdf(const vec3 &wo, const vec3 &wi) const {
        return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * INV_PI : 0;
    }

private:
    const vec3 R; // the fraction of incident light that is scattered
};

class BSDF {
public:
    BSDF(BxDF *b) { 
        bxdf = b; 
    }

    vec3 f(const vec3 &normal, const vec3 &woW, const vec3 &wiW) const {
        mat3 l2w = localToWorld(normal);
        mat3 w2l = inverse(l2w);

        vec3 wi = w2l * wiW;
        vec3 wo = w2l * woW;
        
        vec3 f = bxdf->f(wo, wi);

        return f;
    }

    vec3 rho(int nSamples, const vec2 *samples1, const vec2 *samples2) const;
    vec3 rho(const vec3 &wo, int nSamples, const vec2 *samples) const;

    vec3 Sample_f(const vec3 &normal, const vec3 &woW, vec3 *wiW, const vec2 &u, float *pdf) const {
        mat3 l2w = localToWorld(normal);
        mat3 w2l = inverse(l2w);

        vec3 wi, wo = w2l * woW;
        *pdf = 0;
        
        vec3 f = bxdf->Sample_f(wo, &wi, u, pdf);
        if (*pdf == 0) 
            return vec3(0);
        *wiW = l2w * wi;

        *pdf += bxdf->Pdf(wo, wi);

        // if not specular
        //f = bxdf->f(wo, wi);
        
        return f;
    }

    float Pdf(const vec3 &normal, const vec3 &woW, const vec3 &wiW) const {
        mat3 l2w = localToWorld(normal);
        mat3 w2l = inverse(l2w);

        vec3 wi = w2l * wiW;
        vec3 wo = w2l * woW;

        float pdf = 0;

        pdf += bxdf->Pdf(wo, wi);

        return pdf;
    }

private:
    BxDF *bxdf;
};