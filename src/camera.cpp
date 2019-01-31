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
      distance * -sinYaw * cosPitch + lookAt.x(),
      distance * -sinPitch + lookAt.y(),
      -distance * cosYaw * cosPitch + lookAt.z(),
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
  math::vec3 offset(0, 0, 0);
  if (lensRadius > 0) {
    math::vec3 lensPoint = lensRadius * math::randomPointInUnitDisk();
    offset = right * lensPoint.x() + up * lensPoint.y();
  }
  return {origin + offset,
          lowerLeft + s * horizontal + t * vertical - origin - offset};
}

bool Camera::renderInspector() {
  bool change = false;

  if (ImGui::CollapsingHeader("Position")) {
    change = ImGui::Vec3DragFloat("look at", lookAt, 0.05) || change;
    change = ImGui::DragFloat("pitch", &pitch, 0.05, -89.99, 89.99) || change;
    change = ImGui::DragFloat("yaw", &yaw, 0.05, -89.99, 89.99) || change;
    change = ImGui::DragFloat("distance", &distance, 0.05, 3, 1000) || change;
  }

  if (ImGui::CollapsingHeader("Depth of Field")) {
    change = ImGui::DragFloat("aperture", &aperture, 0.001, 0, 10) || change;
    change = ImGui::DragFloat("focal point", &focusDistance, 0.005) || change;
  }

  if (ImGui::CollapsingHeader("Details")) {
    auto textVec3 = [](const char* name, math::vec3& v) {
      ImGui::Text("%s: ", name);
      ImGui::SameLine();
      ImGui::Indent(85);
      ImGui::Text("[%.3f, %.3f, %.3f]", v.x(), v.y(), v.z());
      ImGui::Unindent(85);
    };

    textVec3("origin", origin);
    textVec3("forward", forward);
    textVec3("right", right);
    textVec3("up", up);
    textVec3("lowerLeft", lowerLeft);
    textVec3("horizontal", horizontal);
    textVec3("vertical", vertical);
  }

  return change;
}

}  // namespace camera
