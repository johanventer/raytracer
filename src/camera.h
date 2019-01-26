namespace camera {

struct Camera : public ImGuiInspectable {
  f32 aspect;
  f32 fov = math::radians(30);
  f32 distance = 10;
  f32 distanceVel = 0;
  f32 yaw = 0;
  f32 pitch = 0;
  f32 aperture = 0;
  f32 focusDistance = 1;

  math::vec3 lookAt = {0, 0, 0};

  math::vec3 origin;
  math::vec3 forward;
  math::vec3 right;
  math::vec3 up;

  math::vec3 lowerLeft;
  math::vec3 horizontal;
  math::vec3 vertical;

  Camera(const u32 screenWidth, const u32 screenHeight)
      : aspect(f32(screenWidth) / f32(screenHeight)) {}

  Camera(const u32 screenWidth,
         const u32 screenHeight,
         const f32 distance,
         const f32 fov,  // NOTE(johan): This is vertical FOV in degrees
         const f32 aperture,
         const f32 focusDistance,
         const f32 pitch,
         const f32 yaw)
      : aspect(f32(screenWidth) / f32(screenHeight)),
        fov(math::radians(fov)),
        distance(distance),
        yaw(yaw),
        pitch(pitch),
        aperture(aperture),
        focusDistance(focusDistance) {
    update(0);
  }

  void update(f64 dt);
  math::Ray ray(const f32 s, const f32 t) const;

  bool renderInspector() override;
};

}  // namespace camera