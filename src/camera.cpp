namespace camera {

void Camera::update(f64 dt) {
  distanceVel *= dt;
  distance += distanceVel;

  // TODO(johan): Clamping the distance to reasonable values for now
  distance = math::clamp(distance, 3, 1000);

  // TODO(johan): Using euler rotations like this causes gimbal lock,
  // clamping the pitch helps
  pitch = math::clamp(pitch, -89.99f, 89.99f);

  f32 cosPitch = cosf(math::radians(pitch));
  f32 sinPitch = sinf(math::radians(pitch));
  f32 cosYaw = cosf(math::radians(yaw));
  f32 sinYaw = sinf(math::radians(yaw));

  // NOTE(johan): Calculate the camera origin
  origin = {
      distance * -sinYaw * cosPitch + lookAt.x,
      distance * -sinPitch + lookAt.y,
      -distance * cosYaw * cosPitch + lookAt.z,
  };

  // NOTE(johan): Calculate the basis axes
  forward = math::normalize(origin - lookAt);
  right = math::normalize(math::cross(math::vec3(0, 1, 0), forward));
  up = math::normalize(math::cross(forward, right));

  // NOTE(johan): Calculate projection axes for generating rays
  f32 halfHeight = tan(fov / 2);
  f32 halfWidth = aspect * halfHeight;
  lowerLeft = origin - halfWidth * focusDistance * right -
              halfHeight * focusDistance * up - focusDistance * forward;
  horizontal = 2 * halfWidth * focusDistance * right;
  vertical = 2 * halfHeight * focusDistance * up;
}

math::Ray Camera::ray(const f32 s, const f32 t) const {
  f32 lensRadius = aperture / 2;
  math::vec3 offset = {0, 0, 0};
  if (lensRadius > 0) {
    math::vec3 lensPoint = math::randomPointInUnitDisk();
    offset = right * lensPoint.x + up * lensPoint.y;
  }
  return {origin + offset,
          lowerLeft + s * horizontal + t * vertical - origin - offset};
}

}  // namespace camera