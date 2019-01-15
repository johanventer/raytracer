namespace Material {

enum class MaterialType { Diffuse, Metal, Dielectric };

struct Diffuse {
  vec3 albedo;

  Diffuse(const vec3& albedo) : albedo(albedo) {}

  bool scatter(const Ray& ray,
               const Hit& hit,
               vec3& attenuation,
               Ray& scattered) const {
    vec3 target = hit.p + hit.normal + randomPointInUnitSphere();
    scattered = Ray(hit.p, target - hit.p);
    attenuation = albedo;
    return true;
  }
};

struct Metal {
  vec3 albedo;
  f32 fuzziness;

  Metal(const vec3& albedo, const f32 fuzziness)
      : albedo(albedo), fuzziness(clamp(fuzziness)) {}

  bool scatter(const Ray& ray,
               const Hit& hit,
               vec3& attenuation,
               Ray& scattered) const {
    vec3 reflected = reflect(ray.direction, hit.normal);
    scattered = Ray(hit.p, reflected + fuzziness * randomPointInUnitSphere());
    attenuation = albedo;
    return (dot(scattered.direction, hit.normal) > 0);
  }
};

struct Dielectric {
  f32 refractiveIndex;

  Dielectric(const f32 refractiveIndex)
      : refractiveIndex(min(1, refractiveIndex)) {}

  bool scatter(const Ray& ray,
               const Hit& hit,
               vec3& attenuation,
               Ray& scattered) const {
    vec3 outwardNormal;
    f32 refractionRatio;
    f32 cosine;
    vec3 refracted;
    vec3 reflected = reflect(ray.direction, hit.normal);
    f32 reflectionProbability = 1;
    f32 rDotN = dot(normalize(ray.direction), hit.normal);

    if (rDotN > 0) {
      outwardNormal = -hit.normal;
      refractionRatio = refractiveIndex;
      cosine = rDotN;
    } else {
      outwardNormal = hit.normal;
      refractionRatio = 1 / refractiveIndex;
      cosine = -rDotN;
    }

    if (refract(ray.direction, outwardNormal, refractionRatio, refracted)) {
      reflectionProbability = schlick(cosine, refractiveIndex);
    }

    if (drand48() < reflectionProbability) {
      scattered = Ray(hit.p, reflected);
    } else {
      scattered = Ray(hit.p, refracted);
    }

    // TODO(johan): Include albedo in Dialectric material to get colored glass
    attenuation = vec3(1, 1, 1);

    return true;
  }
};

struct Material {
  MaterialType type;
  union {
    Diffuse diffuse;
    Metal metal;
    Dielectric dielectric;
  };

  Material(MaterialType type) : type(type) {}

  bool scatter(const Ray& ray,
               const Hit& hit,
               vec3& attenuation,
               Ray& rayScatter) const {
    switch (type) {
      case MaterialType::Diffuse:
        return diffuse.scatter(ray, hit, attenuation, rayScatter);
      case MaterialType::Metal:
        return metal.scatter(ray, hit, attenuation, rayScatter);
      case MaterialType::Dielectric:
        return dielectric.scatter(ray, hit, attenuation, rayScatter);
    };
  }
};

Material* createDiffuse(const vec3 albedo) {
  Material* material = new Material(MaterialType::Diffuse);
  material->diffuse = Diffuse(albedo);
  return material;
}

Material* createMetal(const vec3 albedo, const f32 fuzziness) {
  Material* material = new Material(MaterialType::Metal);
  material->metal = Metal(albedo, fuzziness);
  return material;
}

Material* createDielectric(const f32 refractiveIndex) {
  Material* material = new Material(MaterialType::Dielectric);
  material->dielectric = Dielectric(refractiveIndex);
  return material;
}

}  // namespace Material