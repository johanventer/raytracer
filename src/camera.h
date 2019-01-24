namespace camera {

struct Camera {
  f32 aspect, fov;
  f32 distance, distanceVel;
  f32 yaw, pitch;
  f32 aperture, focusDistance;

  math::vec3 origin;
  math::vec3 lookAt;

  math::vec3 forward;
  math::vec3 right;
  math::vec3 up;

  math::vec3 lowerLeft;
  math::vec3 horizontal;
  math::vec3 vertical;
};

struct Ray {
  math::vec3 origin;
  math::vec3 direction;
};

}  // namespace camera