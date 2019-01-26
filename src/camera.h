namespace camera {

struct Camera : ImGuiInspectable {
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

  Camera() {}

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
        distanceVel(0),
        yaw(yaw),
        pitch(pitch),
        aperture(aperture),
        focusDistance(focusDistance),
        lookAt({0, 0, 0}) {
    update(0);
  }

  void update(f64 dt);
  math::Ray ray(const f32 s, const f32 t) const;

  bool renderInspector() override;
};

}  // namespace camera