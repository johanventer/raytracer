// TODO(johan): This is the hackiest, most error prone serialize/deserialize
// ever! But it works (for now), so whatever...

std::string serialize(math::vec3 v) {
  std::ostringstream os;
  os << v.x << " " << v.y << " " << v.z;
  return os.str();
}

std::string serialize(camera::Camera* camera) {
  std::ostringstream os;
  os << "Camera " << camera->distance << " " << camera->fov << " "
     << camera->aperture << " " << camera->focusDistance << " " << camera->pitch
     << " " << camera->yaw;
  os << " " << serialize(camera->lookAt);
  return os.str();
}

void serialize(texture::Texture& texture, std::ostringstream& os) {
  os << "Texture " << u32(texture.type()) << " ";
  switch (texture.type()) {
    case texture::TextureType::Solid: {
      auto solid = (texture::Solid*)&texture;
      os << serialize(solid->color);
      break;
    }
    case texture::TextureType::Checker: {
      auto checker = (texture::Checker*)&texture;
      os << checker->frequency << " " << serialize(checker->odd) << " "
         << serialize(checker->even);
      break;
    }
    case texture::TextureType::Noise: {
      auto noise = (texture::Noise*)&texture;
      os << serialize(noise->color) << " " << u32(noise->noiseType) << " "
         << noise->amplitude << " " << noise->frequency << " "
         << noise->amplitudeMultiplier << " " << noise->frequencyMultiplier
         << serialize(noise->offset) << " " << noise->depth << " "
         << noise->marbleAmplitude << " " << noise->marbleFrequency;
      break;
    }
    default:
      assert("Bad texture type");
  }
}

void serialize(material::Material& material, std::ostringstream& os) {
  os << "Material " << u32(material.type()) << " ";
  switch (material.type()) {
    case material::MaterialType::Diffuse: {
      // auto diffuse = (material::Diffuse*)&material;
      // Nothing to serialize
      break;
    }
    case material::MaterialType::Metal: {
      auto metal = (material::Metal*)&material;
      os << metal->fuzziness;
      break;
    }
    case material::MaterialType::Dielectric: {
      auto dielectric = (material::Dielectric*)&material;
      os << dielectric->refractiveIndex;
      break;
    }
    default:
      assert("Bad material type");
  }
  if (material.texture) {
    os << " ";
    serialize(*material.texture, os);
  }
}

std::string serialize(entity::Entity& entity) {
  std::ostringstream os;
  os << "Entity " << u32(entity.type()) << " ";
  switch (entity.type()) {
    case entity::EntityType::Sphere: {
      auto sphere = (entity::Sphere*)&entity;
      os << serialize(sphere->center) << " " << sphere->radius;
      break;
    }
    default:
      assert("Bad entity type");
  }
  if (entity.material) {
    os << " ";
    serialize(*entity.material, os);
  }
  return os.str();
}

void serializeScene(std::string fileName,
                    entity::EntityList& entities,
                    camera::Camera* camera) {
  std::ofstream file(fileName);
  if (!file.good()) {
    fatal("Could not open file");
  }
  file << serialize(camera) << "\n";
  for (auto& entity : entities) {
    file << serialize(*entity) << "\n";
  }
  file.close();
}

math::vec3 deserializeVec3(std::ifstream& is) {
  math::vec3 v;
  is >> v.x >> v.y >> v.z;
  return v;
}

camera::Camera* deserializeCamera(std::ifstream& is,
                                  u32 screenWidth,
                                  u32 screenHeight) {
  auto camera = new camera::Camera(screenWidth, screenHeight);
  is >> camera->distance >> camera->fov >> camera->aperture >>
      camera->focusDistance >> camera->pitch >> camera->yaw;
  camera->lookAt = deserializeVec3(is);
  return camera;
}

texture::Texture* deserializeTexture(std::ifstream& is) {
  u32 type;
  is >> type;

  switch (texture::TextureType(type)) {
    case texture::TextureType::Solid: {
      auto solid = new texture::Solid();
      solid->color = deserializeVec3(is);
      return solid;
    }
    case texture::TextureType::Checker: {
      auto checker = new texture::Checker();
      is >> checker->frequency;
      checker->odd = deserializeVec3(is);
      checker->even = deserializeVec3(is);
      return checker;
    }
    case texture::TextureType::Noise: {
      auto noise = new texture::Noise();
      noise->color = deserializeVec3(is);
      u32 noiseType;
      is >> noiseType;
      noise->noiseType = texture::Noise::NoiseType(noiseType);
      is >> noise->amplitude >> noise->frequency >>
          noise->amplitudeMultiplier >> noise->frequencyMultiplier;
      noise->offset = deserializeVec3(is);
      is >> noise->depth >> noise->marbleAmplitude >> noise->marbleFrequency;
      return noise;
    }
    default:
      assert("Bad texture type");
  }
  return nullptr;
}

material::Material* deserializeMaterial(std::ifstream& is) {
  u32 type;
  is >> type;
  switch (material::MaterialType(type)) {
    case material::MaterialType::Diffuse: {
      auto diffuse = new material::Diffuse();
      // nothing to deserialize
      return diffuse;
    }
    case material::MaterialType::Metal: {
      auto metal = new material::Metal();
      is >> metal->fuzziness;
      return metal;
    }
    case material::MaterialType::Dielectric: {
      auto dielectric = new material::Dielectric();
      is >> dielectric->refractiveIndex;
      return dielectric;
    }
    default:
      assert("Bad material type");
  }
  return nullptr;
}

entity::Entity* deserializeEntity(std::ifstream& is) {
  u32 type;
  is >> type;
  switch (entity::EntityType(type)) {
    case entity::EntityType::Sphere: {
      auto sphere = new entity::Sphere();
      sphere->center = deserializeVec3(is);
      is >> sphere->radius;
      return sphere;
    }
    default:
      assert("Bad entity type");
  }
  return nullptr;
}

void deserializeScene(std::string fileName,
                      entity::EntityList& entities,
                      camera::Camera*& camera,
                      u32 screenWidth,
                      u32 screenHeight) {
  std::ifstream file(fileName);
  if (!file.good()) {
    fatal("Could not open scene file");
  }

  entity::Entity* currentEntity = nullptr;
  material::Material* currentMaterial = nullptr;

  while (!file.eof()) {
    std::string type;
    file >> type;

    if (type == "Camera") {
      camera = deserializeCamera(file, screenWidth, screenHeight);
    } else if (type == "Entity") {
      currentEntity = deserializeEntity(file);
      entities.add(currentEntity);
    } else if (type == "Material") {
      currentMaterial = deserializeMaterial(file);
      if (currentEntity) {
        currentEntity->material = currentMaterial;
      } else {
        assert("There should be an entity");
      }
    } else if (type == "Texture") {
      auto texture = deserializeTexture(file);
      if (currentMaterial) {
        currentMaterial->texture = texture;
        currentMaterial = nullptr;
      } else {
        assert("There should be a material");
      }
    }
  }
}
