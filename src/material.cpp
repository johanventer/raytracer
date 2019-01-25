namespace material {

bool Diffuse::scatter(const math::Ray& ray,
                      const Hit& hit,
                      math::vec3& attenuation,
                      math::Ray& scattered) const {
  math::vec3 target = hit.p + hit.normal + math::randomPointInUnitSphere();
  scattered = {hit.p, target - hit.p};
  attenuation = albedo;
  return true;
}

bool Metal::scatter(const math::Ray& ray,
                    const Hit& hit,
                    math::vec3& attenuation,
                    math::Ray& scattered) const {
  math::vec3 reflected = reflect(ray.direction, hit.normal);
  scattered = {hit.p, reflected + fuzziness * math::randomPointInUnitSphere()};
  attenuation = albedo;
  return (dot(scattered.direction, hit.normal) > 0);
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

  // TODO(johan): Include albedo in Dialectric material to get colored glass
  attenuation = {1, 1, 1};

  return true;
}

}  // namespace material