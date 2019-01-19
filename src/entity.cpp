namespace entity {

using Material = material::Material;

bool findHit(const Sphere& sphere,
             const camera::Ray& ray,
             const f32 tMin,
             const f32 tMax,
             Hit& hit) {
  vec3 oc = ray.origin - sphere.center;

  f32 a = dot(ray.direction, ray.direction);
  f32 b = dot(oc, ray.direction);
  f32 c = dot(oc, oc) - sphere.radius * sphere.radius;
  f32 discriminant = b * b - a * c;

  if (discriminant > 0) {
    f32 t = (-b - sqrt(discriminant)) / a;
    if (t < tMax && t > tMin) {
      hit.t = t;
      hit.p = rayAt(ray, t);
      hit.normal = normalize((hit.p - sphere.center) / sphere.radius);
      return true;
    }
  }

  return false;
}

bool findHit(const Entity* entity,
             const camera::Ray& ray,
             const f32 tMin,
             const f32 tMax,
             Hit& hit) {
  hit.material = entity->material;
  switch (entity->type) {
    case EntityType::Sphere:
      return findHit(entity->sphere, ray, tMin, tMax, hit);
  }
}

bool getBoundingBox(const Sphere& sphere, bvh::AABB& box) {
  box = bvh::createAABB(
      sphere.center - vec3{sphere.radius, sphere.radius, sphere.radius},
      sphere.center + vec3{sphere.radius, sphere.radius, sphere.radius});
  return true;
}

bool getBoundingBox(const Entity* entity, bvh::AABB& box) {
  switch (entity->type) {
    case EntityType::Sphere:
      return getBoundingBox(entity->sphere, box);
  }
}

Entity* createSphere(const vec3 center, const f32 radius, Material* material) {
  Entity* result = (Entity*)malloc(sizeof(Entity));
  result->type = EntityType::Sphere;
  result->sphere.center = center;
  result->sphere.radius = radius;
  result->material = material;
  return result;
}

}  // namespace entity
