#pragma once

namespace material {

struct Diffuse : public Scatterable {
  math::vec3 albedo;

  Diffuse(math::vec3 albedo) : albedo(albedo) {}

  bool scatter(const math::Ray& ray,
               const Hit& hit,
               math::vec3& attenuation,
               math::Ray& scattered) const override;
};

struct Metal : public Scatterable {
  math::vec3 albedo;
  f32 fuzziness;

  Metal(math::vec3 albedo, f32 fuzziness)
      : albedo(albedo), fuzziness(fuzziness) {}

  bool scatter(const math::Ray& ray,
               const Hit& hit,
               math::vec3& attenuation,
               math::Ray& scattered) const override;
};

struct Dielectric : public Scatterable {
  f32 refractiveIndex;

  Dielectric(f32 refractiveIndex) : refractiveIndex(refractiveIndex) {}

  bool scatter(const math::Ray& ray,
               const Hit& hit,
               math::vec3& attenuation,
               math::Ray& scattered) const override;
};

}  // namespace material