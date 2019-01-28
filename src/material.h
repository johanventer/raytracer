#pragma once

namespace material {

enum class MaterialType {
  start,
  Diffuse,
  Metal,
  Dielectric,
  DiffuseLight,
  end
};

struct Material : public Scatterable, public Emissive, public ImGuiInspectable {
  INTERFACE(Material);

  virtual MaterialType type() const = 0;
  texture::Texture* texture = nullptr;

  Material(texture::Texture* texture) : texture(texture) {}

  virtual math::vec3 emit(f32 u, f32 v, const math::vec3& p) const {
    return {0, 0, 0};
  }
};

struct Diffuse : public Material {
  Diffuse() {}
  Diffuse(texture::Texture* texture) : Material(texture) {}

  inline MaterialType type() const override { return MaterialType::Diffuse; }

  bool scatter(const math::Ray& ray,
               const Hit& hit,
               math::vec3& attenuation,
               math::Ray& scattered) const override;

  bool renderInspector() override;
};

struct Metal : public Material {
  f32 fuzziness;

  Metal() {}
  Metal(texture::Texture* texture, f32 fuzziness)
      : Material(texture), fuzziness(math::clamp(fuzziness, 0, 1)) {}

  inline MaterialType type() const override { return MaterialType::Metal; }

  bool scatter(const math::Ray& ray,
               const Hit& hit,
               math::vec3& attenuation,
               math::Ray& scattered) const override;

  bool renderInspector() override;
};

struct Dielectric : public Material {
  f32 refractiveIndex;

  Dielectric() {}
  Dielectric(texture::Texture* texture, f32 refractiveIndex)
      : Material(texture),
        refractiveIndex(math::clamp(refractiveIndex, 1, 3)) {}

  Dielectric(f32 refractiveIndex)
      : Dielectric(new texture::Solid({1, 1, 1}), refractiveIndex) {}

  inline MaterialType type() const override { return MaterialType::Dielectric; }

  bool scatter(const math::Ray& ray,
               const Hit& hit,
               math::vec3& attenuation,
               math::Ray& scattered) const override;

  bool renderInspector() override;
};

struct DiffuseLight : public Material {
  DiffuseLight() {}
  DiffuseLight(f32 power) : power(power) {}
  DiffuseLight(texture::Texture* texture, f32 power)
      : Material(texture), power(power) {}

  inline MaterialType type() const override {
    return MaterialType::DiffuseLight;
  }

  f32 power = 4;

  bool scatter(const math::Ray& ray,
               const Hit& hit,
               math::vec3& attenuation,
               math::Ray& scattered) const override;
  math::vec3 emit(f32 u, f32 v, const math::vec3& p) const override;
  bool renderInspector() override;
};

}  // namespace material