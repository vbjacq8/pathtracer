#pragma once
#include<utility>
#include"bvh_node.h"
#include"hitable.h"

/**
 * \brief Bounding Volume Hierarchy implementation to speed up Ray-Hitable collision detections
 */
class BVH : public Hitable{

    public: 
        /**
         * \param primitives pointer to the first Hitable pointer; 
         * \param count number of total primitives to place in tree and detect
         */
        BVH(Hitable** primitives, int count){
            if (count <= 0){return;}
            bvhNodes.resize(count * 2 - 1);
            BVHNode& root = bvhNodes[0];
            root.left = 0;
            root.firstPrimIdx = 0; 
            root.primCount = count;

            primList.assign(primitives, primitives + count);
            primIdxList.resize(count);
            for (int i = 0; i < count; ++i){
                primIdxList[i] = i;
            }

            updateBounds(rootIdx);
            subdivide(rootIdx);
        }


        /**
         * \brief Hitable::hit implementation; delegates to intersectBVH
         */
        bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) override {
            return intersectBVH(0, r, tMin, tMax, hr);
        }

        /**
         * \returns the middle of the root's bounding box 
         */
        vec3 centroid() const override {
            return (bvhNodes[0].aabb.min + bvhNodes[0].aabb.max) / 2;
        }

        /**
         * \returns the root's bounding box 
         */
         AABB boundingBox() const override {
            return bvhNodes[0].aabb;
         }


    private:
        /**
         * \brief Updates the \sa BVHNode.nodeAabb struct to reflect the union of child primitive bounding boxes
         * \param nodeIdx index of node of bvhNodes 
         */
        void updateBounds(int nodeIdx){
            BVHNode& node = bvhNodes[nodeIdx];
            AABB nodeAabb;
            nodeAabb.min = vec3(1e30f, 1e30f, 1e30f);
            nodeAabb.max = -1 * nodeAabb.min;
            for (int first = node.firstPrimIdx, i = 0; i < node.primCount; i++){
                //indirection because we don't want to switch around primitives in the primList, just their indices.
                int idx = primIdxList[first + i];
                Hitable* primitive = primList[idx];
                AABB primBox = primitive->boundingBox();
                nodeAabb.min = min3(nodeAabb.min, primBox.min);
                nodeAabb.max = max3(nodeAabb.max, primBox.max);
            }
            node.aabb = nodeAabb;
        }

        /**
         * \brief Takes non-leaf nodes and recursively partitions their primitives in-place towards left and right nodes
        */
        void subdivide(int nodeIdx){
            BVHNode& node = bvhNodes[nodeIdx];
            if (node.primCount <= 2){return;}
            AABB nodeAabb = node.aabb;
            vec3 extent = nodeAabb.max - nodeAabb.min;
            int axis = 0;
            if (extent.y() > extent.x()) axis = 1;
            if (extent.z() > extent[axis]) axis = 2;
            double splitPos = (nodeAabb.min[axis] + extent[axis]) * 0.5f;

            int i = node.firstPrimIdx;
            int j = node.firstPrimIdx + node.primCount - 1;
            while (i <= j){
                if (primList[primIdxList[i]]->centroid()[axis] > splitPos){
                    std::swap(primIdxList[i], primIdxList[j--]);
                }
                else {i++;}
            }
            int leftCount = i - node.firstPrimIdx;
            if (leftCount == 0 || leftCount == node.primCount){return;}
            int leftChildIdx = nodesUsed++; int rightChildIdx = nodesUsed++;
            bvhNodes[leftChildIdx].firstPrimIdx = node.firstPrimIdx;
            bvhNodes[leftChildIdx].primCount = leftCount;
            bvhNodes[rightChildIdx].firstPrimIdx = i;
            bvhNodes[rightChildIdx].primCount = node.primCount - leftCount;
            node.left = leftChildIdx;
            //set to 0 so that BVHNode::triCount() returns correctly. 
            node.primCount = 0;
            updateBounds(leftChildIdx);
            updateBounds(rightChildIdx);
            //recurse: 
            subdivide(leftChildIdx);
            subdivide(rightChildIdx);

        }

        /**
         * \brief BVH traversal method delegated to by \sa Hit
        */
        bool intersectBVH(int nodeIdx, const Ray& r, double tMin, double tMax, HitRecord& hr){
            BVHNode& node = bvhNodes[nodeIdx];
            if (!node.aabb.intersects(r, tMin, tMax)){return false;}
            bool hitAnything = false;
            double closestSoFar = tMax;
            if (node.isLeaf()){
                for (int i =0; i < node.primCount; ++i){
                    int idx = primIdxList[node.firstPrimIdx + i];
                    if (primList[idx]->hit(r, tMin, closestSoFar, hr)) {
                        closestSoFar = hr.t;
                        hitAnything = true;
                    }
                }
                return hitAnything;
            }

            bool hitLeft = intersectBVH(node.left, r, tMin, closestSoFar, hr);
            if (hitLeft){
                closestSoFar = hr.t;
                hitAnything = true;}
            if (intersectBVH(node.left + 1, r, tMin, closestSoFar, hr)){
                hitAnything = true;
            }
            return hitAnything;

        }


        /** \brief Primitive pointers copied from the constructor argument; never reordered. */
        std::vector<Hitable*> primList;

        /** \brief Permutation of indices into primList; partitioned in-place during subdivide(). */
        std::vector<int> primIdxList;

        /** \brief Flat node pool (at most \c 2*count-1 nodes). Root is index \c 0. */
        std::vector<BVHNode> bvhNodes;

        /** \brief Index of the root node in bvhNodes. */
        int rootIdx = 0;

        /** \brief Next unused node index; incremented by two on each split (left/right pair). */
        int nodesUsed = 1;

        


    



    };