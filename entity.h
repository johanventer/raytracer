// struct AABB;
struct Hit;

namespace entity {

using Material = material::Material;

enum class EntityType { Sphere };

struct Sphere {
  vec3 center;
  f32 radius;
};

struct Entity {
  EntityType type;
  union {
    Sphere sphere;
  };
  Material* material;
};

bool hit(Entity* entity,
         const camera::Ray& ray,
         const f32 tMin,
         const f32 tMax,
         Hit& hit);
// bool boundingBox(Entity* entity, AABB& box);

Entity* createSphere(const vec3 center, const f32 radius, Material* material);

}  // namespace entity