#ifndef CAMERA_H
#define CAMERA_H

#include"my_random.h"
#include"color.h"
#include<functional>

class Camera {
    public: 
        Camera(vec3& origin, vec3& lowerLeftCorner, vec3& vertical, vec3& horizontal)
        {
            this->origin = origin;
            this->lowerLeftCorner = lowerLeftCorner;
            this->vertical = vertical;
            this->horizontal = horizontal;
        }

        /**
         * \brief getter for a ray to a viewport
         * \returns a Ray from the origin to a specified point on the viewport
         * \param u value from (0,1) representing completion of horizontal
         * \param v value from (0,1) representing completion of vertical
         */
        Ray getRay(double u, double v){
            return Ray(origin, lowerLeftCorner + u* horizontal + v * vertical - origin);
        }

        /**
         * \brief anti-aliasing sample method that takes previously discrete (u,v) pairs and
         * samples them around u and v, averaging by \param numSamples number of samples to take
         * \param i ith pixel
         * \param j jth pixel
         * \param nx total horizontal pixels
         * \param ny total vertical pixels
         * \param world pointer to Hitable(s) in world space.
         * \param colorType type of coloring behavior
         */

        /** 
         * \brief coloring strategy used in sampling functions
         * \copydoc see color.h
         */
        using objectColor = std::function<vec3 (const Ray&, Hitable*, int, int)>;

        vec3 colorSample(int i, int j, int nx, int ny, int numSamples, Hitable* world, int depth, objectColor objCol){
            vec3 col(0,0,0);
            for (int k = 0; k < numSamples; k++){
                double randcoeff1 = randomDouble(0.0,1.0);
                double randcoeff2 = randomDouble(0.0,1.0);
                double u = double(i + randcoeff1) / double(nx);
                double v = double(j + randcoeff2) / double(ny);
                Ray r = getRay(u, v);
                col += objCol(r, world, depth, 0);
            }
            col /= numSamples;
            return col;

        }

        
        vec3 metalColorSample(int i, int j, int nx, int ny, int numSamples, Hitable* world, int depth){
            vec3 col(0,0,0);
            for (int k = 0; k < numSamples; k++){
                double randcoeff1 = randomDouble(0.0,1.0);
                double randcoeff2 = randomDouble(0.0,1.0);
                double u = double(i + randcoeff1) / double(nx);
                double v = double(j + randcoeff2) / double(ny);
                Ray r = getRay(u, v);
                col += metalColor(r, world, depth, 0);
            }
            col /= numSamples;
            return col;

        }
        
    
    vec3 origin;
    vec3 lowerLeftCorner;
    vec3 vertical;
    vec3 horizontal;
    };

#endif
            

