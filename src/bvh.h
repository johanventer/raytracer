namespace bvh {

struct AABB {
  math::vec3 minPoint, maxPoint;
};

struct BoundingVolume {
  AABB box;
  BoundingVolume* left;
  BoundingVolume* right;
  EntityList entities;

  BoundingVolume(EntityList& _entities, u32 depth);
};

AABB createAABB(const math::vec3& minPoint, const math::vec3& maxPoint);
AABB surroundingBox(const AABB& box0, const AABB& box1);

}  // namespace bvh
