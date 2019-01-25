namespace bvh {

struct BoundingVolume : public Hitable {
  math::AABB box;
  entity::EntityList entities;
  BoundingVolume* left;
  BoundingVolume* right;

  BoundingVolume(entity::EntityList& _entities);

  bool hit(const math::Ray& ray,
           f32 tMin,
           f32 tMax,
           Hit& resultHit) const override;
};

}  // namespace bvh
