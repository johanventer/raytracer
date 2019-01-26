namespace texture {

const char* toString(TextureType type) {
  switch (type) {
    case TextureType::Solid:
      return "Solid";
    case TextureType::Checker:
      return "Checker";
    case TextureType::Noise:
      return "Noise";
    default:
      assert("Unknown texture type");
  };
  return nullptr;
}

Texture* createTexture(TextureType type) {
  switch (type) {
    case TextureType::Solid:
      return new Solid();
    case TextureType::Checker:
      return new Checker();
    case TextureType::Noise:
      return new Noise();
    default:
      assert("Unknown texture type");
  };
  return nullptr;
}

math::vec3 Solid::sample(f32 u, f32 v, const math::vec3& p) const {
  return color;
}

bool Solid::renderInspector() {
  bool change = false;
  change = ImGui::ColorEdit3("color", color.e) || change;
  return change;
}

math::vec3 Checker::sample(f32 u, f32 v, const math::vec3& p) const {
  f32 sines =
      sin(frequency * p.x) * sin(frequency * p.y) * sin(frequency * p.z);
  return sines < 0 ? odd : even;
}

bool Checker::renderInspector() {
  bool change = false;
  change = ImGui::DragFloat("frequency", &frequency, 1.0f, 1.0f, 100.0f);
  change = ImGui::ColorEdit3("odd", odd.e) || change;
  change = ImGui::ColorEdit3("even", even.e) || change;
  return change;
}

math::vec3 Noise::sample(f32 u, f32 v, const math::vec3& p) const {
  if (spherical) {
    return color * 0.5 * shift *
           (shift + sin(scale * p.z + 10 * perlin.turbulence(p, depth)));
  } else {
    f32 noise = depth > 0 ? perlin.turbulence(scale * p, depth)
                          : perlin.noise(scale * p);
    return color * 0.5 * shift * (shift + noise);
  }
}

bool Noise::renderInspector() {
  bool change = false;
  change = ImGui::DragFloat("scale", &scale, 0.01, 0, 100) || change;
  change = ImGui::DragInt("depth", &depth, 1, 0, 7) || change;
  change = ImGui::DragFloat("shift", &shift, 0.01, 0.01, 2) || change;
  change = ImGui::Checkbox("spherical", &spherical) || change;
  change = ImGui::ColorEdit3("color", color.e) || change;
  return change;
}

}  // namespace texture