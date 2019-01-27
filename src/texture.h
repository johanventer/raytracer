#pragma once

namespace texture {
enum class TextureType { start, Solid, Checker, Noise, Image, end };

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
  enum class NoiseType { Normal, Marble, Wood };

  inline TextureType type() const override { return TextureType::Noise; }
  math::Perlin perlin;
  math::vec3 color = {1, 1, 1};
  NoiseType noiseType = NoiseType::Normal;
  f32 amplitude = 1;
  f32 frequency = 1;
  f32 amplitudeMultiplier = 0.5;
  f32 frequencyMultiplier = 2;
  math::vec3 offset = {0, 0, 0};
  s32 depth = 3;
  f32 marbleAmplitude = 1;
  f32 marbleFrequency = 1;

  Noise() {}

  math::vec3 sample(f32 u, f32 v, const math::vec3& p) const override;
  bool renderInspector() override;
};

struct Image : public Texture {
  inline TextureType type() const override { return TextureType::Image; }
  std::unique_ptr<u8[]> imageData = nullptr;
  s32 width = 0;
  s32 height = 0;
  s32 components = 0;
  std::string imageFile = "";

  Image() {}

  void loadImage(std::string& fileName);
  math::vec3 sample(f32 u, f32 v, const math::vec3& p) const override;
  bool renderInspector() override;
};

}  // namespace texture