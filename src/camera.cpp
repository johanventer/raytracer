namespace camera {

void updateCamera(Camera& camera) {
  // TODO(johan): Clamping the distance to reasonable values for now
  camera.distance = math::clamp(camera.distance, 3, 1000);

  // TODO(johan): Using euler rotations like this causes gimbal lock,
  // clamping the pitch helps
  camera.pitch = math::clamp(camera.pitch, -89.99f, 89.99f);

  f32 cosPitch = cosf(math::radians(camera.pitch));
  f32 sinPitch = sinf(math::radians(camera.pitch));
  f32 cosYaw = cosf(math::radians(camera.yaw));
  f32 sinYaw = sinf(math::radians(camera.yaw));

  // NOTE(johan): Calculate the camera origin
  math::vec3 origin = {
      camera.distance * -sinYaw * cosPitch + camera.lookAt.x,
      camera.distance * -sinPitch + camera.lookAt.y,
      -camera.distance * cosYaw * cosPitch + camera.lookAt.z,
  };

  // NOTE(johan): Calculate the basis axes
  camera.origin = origin;
  camera.forward = math::normalize(camera.origin - camera.lookAt);
  camera.right =
      math::normalize(math::cross(math::vec3(0, 1, 0), camera.forward));
  camera.up = math::normalize(math::cross(camera.forward, camera.right));

  // NOTE(johan): Calculate projection axes for generating rays
  f32 halfHeight = tan(camera.fov / 2);
  f32 halfWidth = camera.aspect * halfHeight;
  camera.lowerLeft = camera.origin -
                     halfWidth * camera.focusDistance * camera.right -
                     halfHeight * camera.focusDistance * camera.up -
                     camera.focusDistance * camera.forward;
  camera.horizontal = 2 * halfWidth * camera.focusDistance * camera.right;
  camera.vertical = 2 * halfHeight * camera.focusDistance * camera.up;
}

Camera* createCamera(
    const u32 screenWidth,
    const u32 screenHeight,
    const f32 distance,
    const f32 fov,  // NOTE(johan): This is vertical FOV in degrees
    const f32 aperture,
    const f32 focusDistance,
    const f32 pitch,
    const f32 yaw) {
  Camera* camera = (Camera*)malloc(sizeof(Camera));

  camera->aspect = f32(screenWidth) / f32(screenHeight);
  camera->fov = math::radians(fov);
  camera->distance = distance;
  camera->pitch = pitch;
  camera->yaw = yaw;
  camera->lookAt = {0, 0, 0};
  camera->focusDistance = focusDistance;
  camera->aperture = aperture;

  updateCamera(*camera);

  return camera;
}

Ray ray(Camera* camera, const f32 s, const f32 t) {
  f32 lensRadius = camera->aperture / 2;
  math::vec3 offset = {0, 0, 0};
  if (lensRadius > 0) {
    math::vec3 lensPoint = math::randomPointInUnitDisk();
    offset = camera->right * lensPoint.x + camera->up * lensPoint.y;
  }
  return {camera->origin + offset, camera->lowerLeft + s * camera->horizontal +
                                       t * camera->vertical - camera->origin -
                                       offset};
}

math::vec3 rayAt(const Ray& ray, const f32 t) {
  return ray.origin + t * ray.direction;
}

}  // namespace camera