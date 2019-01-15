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

Ray ray(Camera* camera, const f32 s, const f32 t);
vec3 rayAt(const Ray& ray, const f32 t);

Camera* createCamera(const vec3 origin,
                     const vec3 lookAt,
                     const vec3 worldUp,
                     const f32 vFov,
                     const f32 aspect,
                     const f32 aperture,
                     const f32 focusDistance);
}  // namespace camera