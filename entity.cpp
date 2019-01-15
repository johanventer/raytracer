namespace Entity {

using Material = Material::Material;

enum class EntityType { Sphere };

struct Sphere {
  vec3 center;
  f32 radius;

  Sphere(vec3 center, f32 radius) : center(center), radius(radius) {}

  bool hit(const Ray& ray, const f32 tMin, const f32 tMax, Hit& hit) const {
    vec3 oc = ray.origin - center;

    f32 a = dot(ray.direction, ray.direction);
    f32 b = dot(oc, ray.direction);
    f32 c = dot(oc, oc) - radius * radius;
    f32 discriminant = b * b - a * c;

    if (discriminant > 0) {
      f32 t = (-b - sqrt(discriminant)) / a;
      if (t < tMax && t > tMin) {
        hit.t = t;
        hit.p = ray.at(t);
        hit.normal = normalize((hit.p - center) / radius);
        return true;
      }
    }

    return false;
  }
};

struct Entity {
  EntityType type;
  union {
    Sphere sphere;
  };
  Material* material;

  Entity(EntityType type, Material* material)
      : type(type), material(material) {}

  bool hit(const Ray& ray, const f32 tMin, const f32 tMax, Hit& hit) const {
    hit.material = material;
    switch (type) {
      case EntityType::Sphere:
        return sphere.hit(ray, tMin, tMax, hit);
    }
  }
};

Entity* createSphere(const vec3 center, const f32 radius, Material* material) {
  Entity* result = new Entity(EntityType::Sphere, material);
  result->sphere = Sphere(center, radius);
  result->material = material;
  return result;
}

}  // namespace Entity
