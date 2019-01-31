namespace texture {

const char* toString(TextureType type) {
  switch (type) {
    case TextureType::Solid:
      return "Solid";
    case TextureType::Checker:
      return "Checker";
    case TextureType::Noise:
      return "Noise";
    case TextureType::Image:
      return "Image";
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
    case TextureType::Image:
      return new Image();
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
  change = ImGui::Vec3ColorEdit("color", color) || change;
  return change;
}

math::vec3 Checker::sample(f32 u, f32 v, const math::vec3& p) const {
  f32 sines =
      sin(frequency * p.x()) * sin(frequency * p.y()) * sin(frequency * p.z());
  return sines < 0 ? odd : even;
}

bool Checker::renderInspector() {
  bool change = false;
  change = ImGui::DragFloat("frequency", &frequency, 1.0f, 1.0f, 100.0f);
  change = ImGui::Vec3ColorEdit("odd", odd) || change;
  change = ImGui::Vec3ColorEdit("even", even) || change;
  return change;
}

math::vec3 Noise::sample(f32 u, f32 v, const math::vec3& p) const {
  if (noiseType == NoiseType::Normal) {
    return color * 0.5 *
           (1 + perlin.turbulence(p, amplitude, frequency, amplitudeMultiplier,
                                  frequencyMultiplier, offset, depth));

  } else if (noiseType == NoiseType::Marble) {
    return color * 0.5 *
           (1 + marbleAmplitude *
                    sin(p.z() + marbleFrequency *
                                    perlin.turbulence(p, amplitude, frequency,
                                                      amplitudeMultiplier,
                                                      frequencyMultiplier,
                                                      offset, depth)));

  } else if (noiseType == NoiseType::Wood) {
    f32 g = perlin.turbulence(p, amplitude, frequency, amplitudeMultiplier,
                              frequencyMultiplier, offset, depth);
    return color * 0.5 * (1 + (g - (s32)g));
  }
  return math::vec3(0, 0, 0);
}

bool Noise::renderInspector() {
  bool change = false;
  change = ImGui::DragInt("depth", &depth, 1, 1, 7) || change;
  change = ImGui::DragFloat("amplitude", &amplitude, 0.01, 0, 1000) || change;
  change = ImGui::DragFloat("frequency", &frequency, 0.01, 0, 1000) || change;
  change = ImGui::DragFloat("amplitude factor", &amplitudeMultiplier, 0.01, 0,
                            1000) ||
           change;
  change = ImGui::DragFloat("frequency factor", &frequencyMultiplier, 0.01, 0,
                            1000) ||
           change;
  change = ImGui::Vec3DragFloat("offset", offset, 0.01, 0, 1000) || change;
  change = ImGui::Vec3ColorEdit("color", color) || change;

  if (ImGui::RadioButton("Normal", noiseType == NoiseType::Normal)) {
    noiseType = NoiseType::Normal;
    change = true;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Marble", noiseType == NoiseType::Marble)) {
    noiseType = NoiseType::Marble;
    change = true;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Wood", noiseType == NoiseType::Wood)) {
    noiseType = NoiseType::Wood;
    change = true;
  }
  if (noiseType == NoiseType::Marble) {
    change =
        ImGui::DragFloat("marble amplitude", &marbleAmplitude, 0.01, 0, 1000) ||
        change;
    change =
        ImGui::DragFloat("marble frequency", &marbleFrequency, 0.01, 0, 1000) ||
        change;
  }

  return change;
}

void Image::loadImage(std::string& fileName) {
  std::ostringstream ss;
  ss << imagesFolder << "/" << fileName;
  u8* data = stbi_load(ss.str().c_str(), &width, &height, &components, 0);
  if (data && components >= 3) {
    imageFile = fileName;
    imageData.reset(data);
  }
}

math::vec3 Image::sample(f32 u, f32 v, const math::vec3& p) const {
  if (!imageData) {
    return {0, 0, 0};
  }

  u32 i = math::clamp(u * width, 0, width - 1);
  u32 j = math::clamp((1 - v) * height - 0.001, 0, height - 1);

  return {int(imageData[components * i + components * width * j]) / 255.0f,
          int(imageData[components * i + components * width * j + 1]) / 255.0f,
          int(imageData[components * i + components * width * j + 2]) / 255.0f};
}

bool Image::renderInspector() {
  bool change = false;

  if (imageData) {
    ImGui::Text("%s", imageFile.c_str());
    ImGui::SameLine();
    if (ImGui::Button("Remove")) {
      width = 0;
      height = 0;
      imageFile = "";
      imageData.reset(nullptr);
      change = true;
    };
  } else {
    ImGui::Button("Load");
    if (ImGui::BeginPopupContextItem(nullptr, 0)) {
      auto imageFiles = findImages();
      for (auto& imageFile : imageFiles) {
        if (ImGui::MenuItem(imageFile.c_str())) {
          loadImage(imageFile);
          change = true;
        }
      }
      ImGui::Indent(ImGui::GetWindowContentRegionWidth() - 50);
      if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

  return change;
}

}  // namespace texture