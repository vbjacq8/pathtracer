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
            //Bitwise AND equivalent to modulo 256
            auto i = static_cast<int>(4 * p.x()) & 255;
            auto j = static_cast<int>(4 * p.y()) & 255;
            auto k = static_cast<int>(4 * p.z()) & 255;
            

            //Bitwise XOR to create a unique hash for the corner
            return randDouble[permX[i] ^ permY[j] ^ permZ[k]];

        }





    private:

        static void perlinGeneratePerm(int *p){
            for (int i = 0; i < pointCount; ++i){p[i] = i;}


                permute(p, pointCount);
            }

        static void permute(int* p, int n){
            for (int i = n-1 ; i > 0; --i){
                int target = randomInt(0, n);
                int tmp = p[i];
                p[i] = p[target];
                p[target] = tmp;
                
            }
        }


        static const int pointCount = 256;
        double randDouble[pointCount];
        int permX[pointCount];
        int permY[pointCount];
        int permZ[pointCount];




};