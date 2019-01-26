namespace texture {

const char* toString(TextureType type) {
  switch (type) {
    case TextureType::Solid:
      return "Solid";
    case TextureType::Checker:
      return "Checker";
    default:
      assert("Unknown texture type");
  };
  return nullptr;
}

Texture* createTexture(TextureType type) {
  switch (type) {
    case TextureType::Solid:
      return new Solid({1, 1, 1});
    case TextureType::Checker:
      return new Checker(10, {0, 0, 0}, {1, 1, 1});
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

}  // namespace texture