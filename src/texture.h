#pragma once

namespace texture {
enum class TextureType { start, Solid, Checker, Noise, end };

struct Texture : public Samplable, public ImGuiInspectable {
  INTERFACE(Texture);

  virtual TextureType type() const = 0;
};

struct Solid : public Texture {
  math::vec3 color = {1, 1, 1};

  inline TextureType type() const override { return TextureType::Solid; }

  Solid() {}
  Solid(math::vec3 color) : color(color) {}

  math::vec3 sample(f32 u, f32 v, const math::vec3& p) const override;
  bool renderInspector() override;
};

struct Checker : public Texture {
  f32 frequency = 10;
  math::vec3 odd = {0, 0, 0};
  math::vec3 even = {1, 1, 1};

  inline TextureType type() const override { return TextureType::Checker; }

  Checker() {}
  Checker(f32 frequency, math::vec3 odd, math::vec3 even)
      : frequency(frequency), odd(odd), even(even) {}

  math::vec3 sample(f32 u, f32 v, const math::vec3& p) const override;
  bool renderInspector() override;
};

struct Noise : public Texture {
  inline TextureType type() const override { return TextureType::Noise; }
  math::Perlin perlin;
  math::vec3 color = {1, 1, 1};

  // NOTE(johan): I have no idea what the right names for perlin levers are,
  // but these can give some nice effects.
  f32 scale = 1;
  s32 depth = 7;
  f32 shift = 1;
  bool spherical = false;

  Noise() {}

  math::vec3 sample(f32 u, f32 v, const math::vec3& p) const override;
  bool renderInspector() override;
};

}  // namespace texture