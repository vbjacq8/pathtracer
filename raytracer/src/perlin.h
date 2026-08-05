#pragma once

#include "my_random.h"

class Perlin {
public:
    Perlin() {
        for (int i = 0; i < pointCount; ++i) {
            randVec[i] = randomInSphere();
        }

        perlinGeneratePerm(permX);
        perlinGeneratePerm(permY);
        perlinGeneratePerm(permZ);
    }

    PATHTRACER_HD float noise(const vec3& p) const {
        const float u = p.x() - floorf(p.x());
        const float v = p.y() - floorf(p.y());
        const float w = p.z() - floorf(p.z());

        const int i = static_cast<int>(floorf(p.x()));
        const int j = static_cast<int>(floorf(p.y()));
        const int k = static_cast<int>(floorf(p.z()));
        vec3 c[2][2][2];

        for (int di = 0; di < 2; ++di) {
            for (int dj = 0; dj < 2; ++dj) {
                for (int dk = 0; dk < 2; ++dk) {
                    c[di][dj][dk] = randVec[permX[(i + di) & 255] ^ permY[(j + dj) & 255] ^
                                            permZ[(k + dk) & 255]];
                }
            }
        }

        return triLinearInterp(c, u, v, w);
    }

    PATHTRACER_HD float turb(const vec3& p, int depth) const {
        float accum = 0;
        float weight = 1.0f;
        vec3 temp = p;
        for (int i = 0; i < depth; ++i) {
            accum += weight * noise(temp);
            weight *= 0.5f;
            temp *= 2;
        }

        return fabsf(accum);
    }

private:
    static void perlinGeneratePerm(int* perm) {
        for (int i = 0; i < pointCount; ++i) {
            perm[i] = i;
        }

        permute(perm, pointCount);
    }

    static void permute(int* perm, int n) {
        for (int i = n - 1; i > 0; --i) {
            int target = randomInt(0, i);
            int tmp = perm[i];
            perm[i] = perm[target];
            perm[target] = tmp;
        }
    }

    PATHTRACER_HD float triLinearInterp(vec3 c[2][2][2], float u, float v, float w) const {
        const float uu = u * u * (3 - 2 * u);
        const float vv = v * v * (3 - 2 * v);
        const float ww = w * w * (3 - 2 * w);

        float accum = 0;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 2; ++k) {
                    vec3 weightv(u - i, v - j, w - k);
                    accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) *
                             (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weightv);
                }
            }
        }
        return accum;
    }

    static const int pointCount = 256;
    vec3 randVec[pointCount];
    int permX[pointCount];
    int permY[pointCount];
    int permZ[pointCount];
};
