namespace bvh {

struct AABB {
  vec3 minPoint, maxPoint;
};

struct BoundingVolume {
  AABB box;
  BoundingVolume* left;
  BoundingVolume* right;
  EntityList entities;

  BoundingVolume(EntityList& _entities, u32 depth);
};

AABB createAABB(const vec3& minPoint, const vec3& maxPoint);
AABB surroundingBox(const AABB& box0, const AABB& box1);

}  // namespace bvh
