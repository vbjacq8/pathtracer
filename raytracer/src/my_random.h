#pragma once
#include<random>
#include"vec3.h"

/** 
 * \brief generate a random double
 * \param min inclusive start range
 * \param max exclusive end range
 */
inline double randomDouble(double min, double max){
    std::random_device rd;
    static std::mt19937_64 engine(rd());
    static std::uniform_real_distribution<> dist(0, 1.0);
    return min + (max - min) * dist(engine);
}

/**
 * \brief generates random point vector from center of sphere to a point within using rejection algorithm
 * \returns unit vector satisfying location within sphere
 */
inline vec3 randomInSphere(){
    vec3 p;
    do {
        p = 2.0 * vec3(randomDouble(0.0,1.0), randomDouble(0.0,1.0), randomDouble(0.0,1.0)) - vec3(1,1,1);
    } while (p.squared_norm() >= 1.0);
    return p;
}

inline vec3 randomInDisc(){
    vec3 p;
    do {
        p = 2.0 * vec3(randomDouble(0.0,1.0), randomDouble(0.0,1.0), 0) - vec3(1,1,0);
    } while(p.squared_norm() >= 1.0);
    return p;
}