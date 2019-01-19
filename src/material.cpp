namespace material {

bool scatter(Diffuse& diffuse,
             const camera::Ray& ray,
             const Hit& hit,
             vec3& attenuation,
             camera::Ray& scattered) {
  vec3 target = hit.p + hit.normal + randomPointInUnitSphere();
  scattered = {hit.p, target - hit.p};
  attenuation = diffuse.albedo;
  return true;
}

bool scatter(Metal& metal,
             const camera::Ray& ray,
             const Hit& hit,
             vec3& attenuation,
             camera::Ray& scattered) {
  vec3 reflected = reflect(ray.direction, hit.normal);
  scattered = {hit.p, reflected + metal.fuzziness * randomPointInUnitSphere()};
  attenuation = metal.albedo;
  return (dot(scattered.direction, hit.normal) > 0);
}

bool scatter(Dielectric& dielectric,
             const camera::Ray& ray,
             const Hit& hit,
             vec3& attenuation,
             camera::Ray& scattered) {
  vec3 outwardNormal;
  f32 refractionRatio;
  f32 cosine;
  vec3 refracted;
  vec3 reflected = reflect(ray.direction, hit.normal);
  f32 reflectionProbability = 1;
  f32 rDotN = dot(normalize(ray.direction), hit.normal);

  if (rDotN > 0) {
    outwardNormal = -hit.normal;
    refractionRatio = dielectric.refractiveIndex;
    cosine = rDotN;
  } else {
    outwardNormal = hit.normal;
    refractionRatio = 1 / dielectric.refractiveIndex;
    cosine = -rDotN;
  }

  if (refract(ray.direction, outwardNormal, refractionRatio, refracted)) {
    reflectionProbability = schlick(cosine, dielectric.refractiveIndex);
  }

  if (rand01() < reflectionProbability) {
    scattered = {hit.p, reflected};
  } else {
    scattered = {hit.p, refracted};
  }

  // TODO(johan): Include albedo in Dialectric material to get colored glass
  attenuation = {1, 1, 1};

  return true;
}

bool scatter(Material* material,
             const camera::Ray& ray,
             const Hit& hit,
             vec3& attenuation,
             camera::Ray& rayScatter) {
  switch (material->type) {
    case MaterialType::Diffuse:
      return scatter(material->diffuse, ray, hit, attenuation, rayScatter);
    case MaterialType::Metal:
      return scatter(material->metal, ray, hit, attenuation, rayScatter);
    case MaterialType::Dielectric:
      return scatter(material->dielectric, ray, hit, attenuation, rayScatter);
  };
}

Material* createDiffuse(const vec3 albedo) {
  Material* material = (Material*)malloc(sizeof(Material));
  material->type = MaterialType::Diffuse;
  material->diffuse.albedo = albedo;
  return material;
}

Material* createMetal(const vec3 albedo, const f32 fuzziness) {
  Material* material = (Material*)malloc(sizeof(Material));
  material->type = MaterialType::Metal;
  material->metal.albedo = albedo;
  material->metal.fuzziness = clamp(fuzziness);
  return material;
}

Material* createDielectric(const f32 refractiveIndex) {
  Material* material = (Material*)malloc(sizeof(Material));
  material->type = MaterialType::Dielectric;
  material->dielectric.refractiveIndex = max(1, refractiveIndex);
  return material;
}

}  // namespace material