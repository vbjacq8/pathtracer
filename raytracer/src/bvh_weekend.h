#pragma once
#include <algorithm>
#include <utility>
#include <vector>
#include "bvh_node.h"
#include "hitable.h"

/**
 * \brief Bounding Volume Hierarchy with object-median splits (RTIOW-style).
 */
class BVHWeekend : public Hitable{

    public: 
        /**
         * \param primitives primitives to place in the tree (shared ownership retained)
         */
        explicit BVHWeekend(std::vector<HitablePtr> primitives) : primList(std::move(primitives)) {
            const int count = static_cast<int>(primList.size());
            if (count <= 0){return;}
            bvhNodes.resize(count * 2 - 1);
            BVHNode& root = bvhNodes[0];
            root.left = 0;
            root.firstPrimIdx = 0; 
            root.primCount = count;

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
            if (node.primCount <= 0) {
                return;
            }
            int idx0 = primIdxList[node.firstPrimIdx];
            AABB nodeAabb = primList[idx0]->boundingBox();
            for (int i = 1; i < node.primCount; i++){
                //indirection because we don't want to switch around primitives in the primList, just their indices.
                int idx = primIdxList[node.firstPrimIdx + i];
                nodeAabb = AABB(nodeAabb, primList[idx]->boundingBox());
            }
            node.aabb = nodeAabb;
        }

        /**
         * \brief Object-median split: nth_element on longest axis, then recurse on both halves.
        */
        void subdivide(int nodeIdx){
            BVHNode& node = bvhNodes[nodeIdx];
            if (node.primCount <= 2){return;}

            const AABB& nodeAabb = node.aabb;
            vec3 extent = nodeAabb.max - nodeAabb.min;
            int axis = 0;
            if (extent.y() > extent.x()) axis = 1;
            if (extent.z() > extent[axis]) axis = 2;

            const int first = node.firstPrimIdx;
            const int count = node.primCount;
            const int mid = first + count / 2;

            auto byCentroid = [this, axis](int a, int b) {
                return primList[a]->centroid()[axis] < primList[b]->centroid()[axis];
            };
            std::nth_element(
                primIdxList.begin() + first,
                primIdxList.begin() + mid,
                primIdxList.begin() + first + count,
                byCentroid);

            const int leftCount = mid - first;
            const int rightCount = count - leftCount;
            // Guaranteed for count >= 3: leftCount >= 1 and rightCount >= 1.

            const int leftChildIdx = nodesUsed++;
            const int rightChildIdx = nodesUsed++;
            bvhNodes[leftChildIdx].firstPrimIdx = first;
            bvhNodes[leftChildIdx].primCount = leftCount;
            bvhNodes[rightChildIdx].firstPrimIdx = mid;
            bvhNodes[rightChildIdx].primCount = rightCount;
            node.left = leftChildIdx;
            //set to 0 so that BVHNode::isLeaf() returns false.
            node.primCount = 0;
            updateBounds(leftChildIdx);
            updateBounds(rightChildIdx);
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


        /** \brief Owned primitive references; never reordered. */
        std::vector<HitablePtr> primList;

        /** \brief Permutation of indices into primList; partitioned in-place during subdivide(). */
        std::vector<int> primIdxList;

        /** \brief Flat node pool (at most \c 2*count-1 nodes). Root is index \c 0. */
        std::vector<BVHNode> bvhNodes;

        /** \brief Index of the root node in bvhNodes. */
        int rootIdx = 0;

        /** \brief Next unused node index; incremented by two on each split (left/right pair). */
        int nodesUsed = 1;

    };
