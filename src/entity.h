namespace entity {

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
  material::Material* material;
};

}  // namespace entity