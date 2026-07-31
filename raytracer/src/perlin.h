#pragma once

#include "my_random.h"


class Perlin {
    public:
        Perlin() {
            for (int i =0 ; i < pointCount; ++i){
                randVec[i] = randomInSphere();}

            perlinGeneratePerm(permX);
            perlinGeneratePerm(permY);
            perlinGeneratePerm(permZ);

        }

        float noise(const vec3& p) const {
            //"fractional" pieces of each coordinate
            auto u = p.x() - std::floor(p.x());
            auto v = p.y() - std::floor(p.y());
            auto w = p.z() - std::floor(p.z());
            
            //"whole" integer coordinates
            auto i = static_cast<int>(std::floor(p.x()));
            auto j = static_cast<int>(std::floor(p.y()));
            auto k = static_cast<int>(std::floor(p.z()));
            vec3 c[2][2][2];

            //creates a 2x2x2 3D array of the integer coordinates + perturbation then mapped by modulo 256 
            for (int di = 0; di < 2; ++di){
                for (int dj = 0; dj < 2; ++dj){
                    for (int dk = 0; dk < 2; ++dk){
                        c[di][dj][dk] = randVec[
                            permX[(i + di) & 255] ^
                            permY[(j + dj) & 255] ^
                            permZ[(k + dk) & 255]
                        ];
                    }

                }
            }

            return triLinearInterp(c, u, v, w);

        }

        float turb(const vec3& p, int depth) const {
            float accum = 0;
            float weight = 1.0;
            vec3 temp = p;
            for (int i = 0; i < depth; ++i){
                accum += weight * noise(temp);
                weight *= 0.5;
                temp *= 2;
            }

            return std::fabs(accum);
        }





    private:

        static void perlinGeneratePerm(int *perm){
            for (int i = 0; i < pointCount; ++i){perm[i] = i;}


                permute(perm, pointCount);
            }

        static void permute(int* perm, int n){
            for (int i = n-1 ; i > 0; --i){
                int target = randomInt(0, i);
                int tmp = perm[i];
                perm[i] = perm[target];
                perm[target] = tmp;
                
            }
        }

        float triLinearInterp(vec3 c[2][2][2], float u, float v, float w) const {
            float uu = u * u * (3-2*u);
            float vv = v * v * (3-2*v);
            float ww = w * w * (3-2*w);

            float accum = 0;
            for (int i = 0; i < 2; ++i){
                for (int j = 0; j < 2; ++j){
                    for (int k = 0; k < 2; ++k){
                        vec3 weightv(u - i, v-j, w-k);
                        accum += 
                            (i * uu + (1-i)*(1-uu)) *
                            (j * vv + (1-j)*(1-vv)) * 
                            (k * ww + (1-k)*(1-ww)) * 
                            dot(c[i][j][k], weightv);
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