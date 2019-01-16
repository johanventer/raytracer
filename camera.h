namespace camera {

struct Camera {
  vec3 origin;
  vec3 lowerLeft;
  vec3 horizontal;
  vec3 vertical;
  f32 lensRadius;
  vec3 forward;
  vec3 left;
  vec3 up;
};

struct Ray {
  vec3 origin;
  vec3 direction;
};

}  // namespace camera