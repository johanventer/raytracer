#pragma once

namespace texture {
enum class TextureType { start, Solid, Checker, end };

struct Texture : public Samplable, public ImGuiInspectable {
  INTERFACE(Texture);

  virtual TextureType type() const = 0;
};

struct Solid : public Texture {
  math::vec3 color;

  inline TextureType type() const override { return TextureType::Solid; }

  Solid(math::vec3 color) : color(color) {}

  math::vec3 sample(f32 u, f32 v, const math::vec3& p) const override;
  bool renderInspector() override;
};

struct Checker : public Texture {
  f32 frequency;
  math::vec3 odd, even;

  inline TextureType type() const override { return TextureType::Checker; }

  Checker(f32 frequency, math::vec3 odd, math::vec3 even)
      : frequency(frequency), odd(odd), even(even) {}

  math::vec3 sample(f32 u, f32 v, const math::vec3& p) const override;
  bool renderInspector() override;
};

}  // namespace texture