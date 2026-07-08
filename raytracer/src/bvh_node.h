#pragma once

#include"vec3.h"
#include"aabb.h"

/**
 * \brief Node of bounding volume hierarchy binary tree.
 * \copydoc BVH
*/
struct BVHNode {
    AABB aabb;
    int left;
    // markers for where the node's primitives lie; 
    // firstPrimIdx is the indice to the indice in primIdxList that points to the node's first child.
    int firstPrimIdx, primCount;
    bool isLeaf(){return primCount > 0;}
};


