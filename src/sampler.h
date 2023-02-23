#include <algorithm>
#include <iostream>

#include "math.h"

// 1.111...111 * 2^(-1)
static const float OneMinusEpsilon = 0x1.fffffep-1;

static uint32_t XorShift32(uint32_t& state) {
    uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 15;
    state = x;
    return x;
}

// with threshold
uint32_t XorShift32(uint32_t& state, uint32_t b) {
    uint32_t threshold = (~b + 1u) % b;
    while (true) {
        uint32_t r = XorShift32(state);
        if (r >= threshold) return r % b;
    }
}

float RandomFloat01(uint32_t& state) {
    return (XorShift32(state) & 0xFFFFFF) / 16777216.0f;
}

void StratifiedSample1D(float *samp, int nSamples, uint32_t& state, bool jitter) {
    float invNSamples = (float)1 / nSamples;
    for (int i = 0; i < nSamples; ++i) {
        float delta = jitter ? RandomFloat01(state) : 0.5f;
        samp[i] = std::min((i + delta) * invNSamples, OneMinusEpsilon);
    }
}

void StratifiedSample2D(vec2 *samp, int nx, int ny, uint32_t& state, bool jitter) {
    float dx = (float)1 / nx, dy = (float)1 / ny;
    for (int y = 0; y < ny; ++y)
        for (int x = 0; x < nx; ++x) {
            float jx = jitter ? RandomFloat01(state) : 0.5f;
            float jy = jitter ? RandomFloat01(state) : 0.5f;
            samp->x = std::min((x + jx) * dx, OneMinusEpsilon);
            samp->y = std::min((y + jy) * dy, OneMinusEpsilon);
            ++samp;
        }
}

// default jittered
void LatinHypercube(float *samples, int nSamples, int nDim, uint32_t& state) {
    float invNSamples = (float)1 / nSamples;
    for (int i = 0; i < nSamples; ++i)
        for (int j = 0; j < nDim; ++j) {
            float sj = (i + (RandomFloat01(state))) * invNSamples;
            samples[nDim * i + j] = std::min(sj, OneMinusEpsilon);
        }

    // shuffle
    for (int i = 0; i < nDim; ++i) {
        for (int j = 0; j < nSamples; ++j) {
            int other = j + XorShift32(state, nSamples - j);
            std::swap(samples[nDim * j + i], samples[nDim * other + i]);
        }
    }
}

template <typename T>
void Shuffle(T *samp, int count, int nDimensions, uint32_t& state) {
    for (int i = 0; i < count; ++i) {
        int other = i + XorShift32(state, count - i);
        for (int j = 0; j < nDimensions; ++j) {
            std::swap(samp[nDimensions * i + j],
                    samp[nDimensions * other + j]);
        }
    }
}

class Sampler {
public:
    Sampler() {
        state = 666;
    }

    Sampler(uint32_t seed) {
        state = seed;
    }

    std::vector<float> sample1D(int nSamples) {
        std::vector<float> samples1D(nSamples);
        StratifiedSample1D(&samples1D[0], nSamples, state, true);
        Shuffle(&samples1D[0], nSamples, 1, state);

        return samples1D;
    }

    std::vector<vec2> sample2D(int nSamples) {
        std::vector<vec2> samples2D(nSamples);
        LatinHypercube(&samples2D[0].x, nSamples, 2, state);

        return samples2D;
    }

    uint32_t state; // cannot be 0
};