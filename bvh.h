namespace bvh {

struct AABB {
  vec3 minPoint, maxPoint;
};

bool findHit(AABB& aabb, const camera::Ray& ray, f32 tMin, f32 tMax);
AABB surroundingBox(const AABB& box0, const AABB& box1);
AABB createAABB(vec3 minPoint, vec3 maxPoint);

struct BoundingVolume {
  AABB box;
  EntityList left;
  EntityList right;
};

}  // namespace bvh
