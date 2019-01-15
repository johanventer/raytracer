namespace camera {
Camera* createCamera(const vec3 origin,
                     const vec3 lookAt,
                     const vec3 worldUp,
                     const f32 vFov,
                     const f32 aspect,
                     const f32 aperture,
                     const f32 focusDistance) {
  f32 theta = vFov * M_PI / 180;
  f32 halfHeight = tan(theta / 2);
  f32 halfWidth = aspect * halfHeight;

  Camera* camera = (Camera*)malloc(sizeof(Camera));

  camera->origin = origin;
  camera->forward = normalize(origin - lookAt);
  camera->left = normalize(cross(worldUp, camera->forward));
  camera->up = cross(camera->forward, camera->left);

  camera->lowerLeft = origin - halfWidth * focusDistance * camera->left -
                      halfHeight * focusDistance * camera->up -
                      focusDistance * camera->forward;
  camera->horizontal = 2 * halfWidth * focusDistance * camera->left;
  camera->vertical = 2 * halfHeight * focusDistance * camera->up;

  camera->lensRadius = aperture / 2;

  return camera;
}

Ray ray(Camera* camera, const f32 s, const f32 t) {
  vec3 rayDirection = camera->lensRadius * randomPointInUnitDisk();
  vec3 offset = camera->left * rayDirection.x + camera->up * rayDirection.y;
  return {camera->origin + offset, camera->lowerLeft + s * camera->horizontal +
                                       t * camera->vertical - camera->origin -
                                       offset};
}

vec3 rayAt(const Ray& ray, const f32 t) {
  return ray.origin + t * ray.direction;
}

}