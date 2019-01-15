#pragma once

struct Hit;

namespace material {

enum class MaterialType { Diffuse, Metal, Dielectric };

struct Diffuse {
  vec3 albedo;
};

struct Metal {
  vec3 albedo;
  f32 fuzziness;
};

struct Dielectric {
  f32 refractiveIndex;
};

struct Material {
  MaterialType type;
  union {
    Diffuse diffuse;
    Metal metal;
    Dielectric dielectric;
  };
};

Material* createDiffuse(const vec3 albedo);
Material* createMetal(const vec3 albedo, const f32 fuzziness);
Material* createDielectric(const f32 refractiveIndex);
bool scatter(Material* material,
             const Ray& ray,
             const Hit& hit,
             vec3& attenuation,
             Ray& rayScatter);

}  // namespace material