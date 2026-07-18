#pragma once

#include "my_random.h"


class Perlin {
    public:
        Perlin() {
            for (int i =0 ; i < pointCount; ++i){randDouble[i] = randomDouble(0, 1);}

            perlinGeneratePerm(permX);
            perlinGeneratePerm(permY);
            perlinGeneratePerm(permZ);

        }

        double noise(const vec3& p) const {
            //"fractional" pieces of each coordinate
            auto u = p.x() - std::floor(p.x());
            auto v = p.y() - std::floor(p.y());
            auto w = p.z() - std::floor(p.z());


            //"whole" integer coordinates
            auto i = static_cast<int>(std::floor(p.x()));
            auto j = static_cast<int>(std::floor(p.y()));
            auto k = static_cast<int>(std::floor(p.z()));
            double c[2][2][2];

            //creates a 2x2x2 3D array of the integer coordinates + perturbation then mapped by modulo 256 
            for (int di = 0; di < 2; ++di){
                for (int dj = 0; dj < 2; ++dj){
                    for (int dk = 0; dk < 2; ++dk){
                        c[di][dj][dk] = randDouble[
                            permX[(i + di) & 255] ^
                            permY[(j + dj) & 255] ^
                            permZ[(k + dk) & 255]
                        ];
                    }

                }
            }

            return triLinearInterp(c, u, v, w);

        }





    private:

        static void perlinGeneratePerm(int *perm){
            for (int i = 0; i < pointCount; ++i){perm[i] = i;}


                permute(perm, pointCount);
            }

        static void permute(int* perm, int n){
            for (int i = n-1 ; i > 0; --i){
                int target = randomInt(0, n);
                int tmp = perm[i];
                perm[i] = perm[target];
                perm[target] = tmp;
                
            }
        }

        double triLinearInterp(double c[2][2][2], double u, double v, double w) const {
            double accum = 0;
            for (int i = 0; i < 2; ++i){
                for (int j = 0; j < 2; ++j){
                    for (int k = 0; k < 2; ++k){
                        accum += 
                            (i * u + (1-i)*(1-u)) *
                            (j * v + (1-j)*(1-v)) * 
                            (k * w + (1-k)*(1-w)) * c[i][j][k];


                    }
                }
            }
            return accum;

        }


        static const int pointCount = 256;
        double randDouble[pointCount];
        int permX[pointCount];
        int permY[pointCount];
        int permZ[pointCount];




};