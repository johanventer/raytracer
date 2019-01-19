namespace camera {

void updateCamera(Camera& camera) {
  f32 halfHeight = tan(camera.fov / 2);
  f32 halfWidth = camera.aspect * halfHeight;

  // NOTE(johan): Update camera movement
  if (camera.pitch > 89.0f)
    camera.pitch = 89.0f;
  if (camera.pitch < -89.0f)
    camera.pitch = -89.0f;

  camera.front =
      normalize({cosf(radians(camera.pitch)) * cosf(radians(camera.yaw)),
                 sinf(radians(camera.pitch)),
                 cosf(radians(camera.pitch)) * sinf(radians(camera.yaw))});

  camera.lookAt = camera.origin + camera.front;

  // NOTE(johan): Calculate camera axes
  camera.forward = normalize(camera.origin - camera.lookAt);
  camera.right = normalize(cross(camera.worldUp, camera.forward));
  camera.up = normalize(cross(camera.forward, camera.right));

  // NOTE(johan): Calculate projection axes for generating rays
  camera.lowerLeft = camera.origin -
                     halfWidth * camera.focusDistance * camera.right -
                     halfHeight * camera.focusDistance * camera.up -
                     camera.focusDistance * camera.forward;
  camera.horizontal = 2 * halfWidth * camera.focusDistance * camera.right;
  camera.vertical = 2 * halfHeight * camera.focusDistance * camera.up;
}

Camera* createCamera(const vec3 origin,
                     const vec3 lookAt,
                     const vec3 worldUp,
                     const u32 width,
                     const u32 height,
                     const f32 fov,
                     const f32 aperture,
                     const f32 focusDistance) {
  Camera* camera = (Camera*)malloc(sizeof(Camera));

  camera->origin = origin;
  // camera->lookAt = lookAt;
  camera->worldUp = worldUp;

  camera->aspect = f32(width) / f32(height);
  camera->fov = radians(fov);
  camera->lensRadius = aperture / 2;
  camera->focusDistance = focusDistance;
  camera->yaw = 0;
  camera->pitch = 0;

  updateCamera(*camera);

  return camera;
}

Ray ray(Camera* camera, const f32 s, const f32 t) {
  auto offset = vec3{0, 0, 0};
  if (camera->lensRadius) {
    vec3 lensPoint = camera->lensRadius * randomPointInUnitDisk();
    offset = camera->right * lensPoint.x + camera->up * lensPoint.y;
  }
  return {camera->origin + offset, camera->lowerLeft + s * camera->horizontal +
                                       t * camera->vertical - camera->origin -
                                       offset};
}

vec3 rayAt(const Ray& ray, const f32 t) {
  return ray.origin + t * ray.direction;
}

}  // namespace camera