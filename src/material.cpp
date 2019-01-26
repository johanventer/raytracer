namespace material {

const char* toString(MaterialType type) {
  switch (type) {
    case MaterialType::Diffuse:
      return "Diffuse";
    case MaterialType::Metal:
      return "Metal";
    case MaterialType::Dielectric:
      return "Dielectric";
    default:
      assert("Unknown material type");
  };
  return nullptr;
}

Material* createMaterial(MaterialType type) {
  switch (type) {
    case MaterialType::Diffuse:
      return new Diffuse(new texture::Solid({1, 1, 1}));
    case MaterialType::Metal:
      return new Metal(new texture::Solid({1, 1, 1}), 0);
    case MaterialType::Dielectric:
      return new Dielectric(new texture::Solid({1, 1, 1}), 1);
    default:
      assert("Unknown material type");
  };
  return nullptr;
}

bool Diffuse::scatter(const math::Ray& ray,
                      const Hit& hit,
                      math::vec3& attenuation,
                      math::Ray& scattered) const {
  math::vec3 target = hit.p + hit.normal + math::randomPointInUnitSphere();
  scattered = {hit.p, target - hit.p};
  if (texture) {
    attenuation = texture->sample(0, 0, hit.p);
  } else {
    attenuation = {1, 1, 1};
  }
  return true;
}

bool Diffuse::renderInspector() {
  bool change = false;
  return change;
}

bool Metal::scatter(const math::Ray& ray,
                    const Hit& hit,
                    math::vec3& attenuation,
                    math::Ray& scattered) const {
  math::vec3 reflected = reflect(ray.direction, hit.normal);
  scattered = {hit.p, reflected + fuzziness * math::randomPointInUnitSphere()};
  if (texture) {
    attenuation = texture->sample(0, 0, hit.p);
  } else {
    attenuation = {1, 1, 1};
  }
  return (dot(scattered.direction, hit.normal) > 0);
}

bool Metal::renderInspector() {
  bool change = false;
  change = ImGui::DragFloat("fuzziness", &fuzziness, 0.01, 0, 1) || change;
  return change;
}

bool Dielectric::scatter(const math::Ray& ray,
                         const Hit& hit,
                         math::vec3& attenuation,
                         math::Ray& scattered) const {
  math::vec3 outwardNormal;
  f32 refractionRatio;
  f32 cosine;
  math::vec3 refracted;
  math::vec3 reflected = math::reflect(ray.direction, hit.normal);
  f32 reflectionProbability = 1;
  f32 rDotN = math::dot(math::normalize(ray.direction), hit.normal);

  if (rDotN > 0) {
    outwardNormal = -hit.normal;
    refractionRatio = refractiveIndex;
    cosine = rDotN;
  } else {
    outwardNormal = hit.normal;
    refractionRatio = 1 / refractiveIndex;
    cosine = -rDotN;
  }

  if (math::refract(ray.direction, outwardNormal, refractionRatio, refracted)) {
    reflectionProbability = math::schlick(cosine, refractiveIndex);
  }

  if (math::rand01() < reflectionProbability) {
    scattered = {hit.p, reflected};
  } else {
    scattered = {hit.p, refracted};
  }

  if (texture) {
    attenuation = texture->sample(0, 0, hit.p);
  } else {
    attenuation = {1, 1, 1};
  }

  return true;
}

bool Dielectric::renderInspector() {
  bool change = false;
  change = ImGui::DragFloat("refractive index", &refractiveIndex, 0.01, 1, 3) ||
           change;
  return change;
}

}  // namespace material