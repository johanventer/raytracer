#pragma once

namespace material {

enum class MaterialType { Diffuse, Metal, Dielectric };

struct Diffuse {
  math::vec3 albedo;
};

struct Metal {
  math::vec3 albedo;
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

}  // namespace material