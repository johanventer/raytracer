namespace camera {

struct Camera {
  vec3 origin;
  vec3 front;
  vec3 lookAt;
  vec3 worldUp;

  f32 aspect, fov;
  f32 yaw, pitch;
  f32 lensRadius, focusDistance;

  vec3 forward;
  vec3 right;
  vec3 up;

  vec3 lowerLeft;
  vec3 horizontal;
  vec3 vertical;
};

struct Ray {
  vec3 origin;
  vec3 direction;
};

}  // namespace camera