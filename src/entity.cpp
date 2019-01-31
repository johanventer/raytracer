namespace entity {

const char* toString(EntityType type) {
  switch (type) {
    case EntityType::Sphere:
      return "Sphere";
    case EntityType::XYRectangle:
      return "XY Rectangle";
    case EntityType::XZRectangle:
      return "XZ Rectangle";
    case EntityType::YZRectangle:
      return "YZ Rectangle";
    case EntityType::FlipNormals:
      return "Flip Normals";
    case EntityType::Box:
      return "Box";
    default:
      assert("Unknown entity type");
  };
  return nullptr;
}

std::string toString(const Sphere* entity) {
  std::ostringstream ss;
  ss << "Sphere " << entity->center;
  return ss.str();
}

std::string toString(const XYRectangle* entity) {
  std::ostringstream ss;
  ss << "XY Rectangle " << entity->center;
  return ss.str();
}

std::string toString(const XZRectangle* entity) {
  std::ostringstream ss;
  ss << "XZ Rectangle " << entity->center;
  return ss.str();
}

std::string toString(const YZRectangle* entity) {
  std::ostringstream ss;
  ss << "YZ Rectangle " << entity->center;
  return ss.str();
}

std::string toString(const FlipNormals* entity) {
  std::ostringstream ss;
  ss << "Flip Normals";
  return ss.str();
}

std::string toString(const Box* entity) {
  std::ostringstream ss;
  ss << "Box " << entity->center;
  return ss.str();
}

std::string toString(const Entity* entity) {
  switch (entity->type()) {
    case EntityType::Sphere:
      return toString((const Sphere*)entity);
    case EntityType::XYRectangle:
      return toString((const XYRectangle*)entity);
    case EntityType::XZRectangle:
      return toString((const XZRectangle*)entity);
    case EntityType::YZRectangle:
      return toString((const YZRectangle*)entity);
    case EntityType::FlipNormals:
      return toString((const FlipNormals*)entity);
    case EntityType::Box:
      return toString((const Box*)entity);
    default:
      assert("Unknown entity type");
  };
  return nullptr;
}

Entity* createEntity(EntityType type) {
  switch (type) {
    case EntityType::Sphere:
      return new Sphere();
    case EntityType::XYRectangle:
      return new XYRectangle();
    case EntityType::XZRectangle:
      return new XZRectangle();
    case EntityType::YZRectangle:
      return new YZRectangle();
    case EntityType::FlipNormals:
      return new FlipNormals();
    case EntityType::Box:
      return new Box();
    default:
      assert("Unknown entity type");
  };
  return nullptr;
}

EntityList EntityList::sort(u32 axis) const {
  EntityList result;
  result.entities.reserve(entities.size());
  result.entities = entities;

  auto sortEntities = [](const entity::Entity* a, const entity::Entity* b,
                         u32 axis) {
    math::AABB aBox, bBox;
    if (!a->bounds(aBox) || !b->bounds(bBox)) {
      fatal("No bounding boxes during comparison");
    }
    return (aBox.minPoint[axis] - bBox.minPoint[axis]) < 0.0;
  };

  std::sort(
      std::begin(result.entities), std::end(result.entities),
      [&axis, &sortEntities](const entity::Entity* a, const entity::Entity* b) {
        return sortEntities(a, b, axis);
      });

  return result;
}

void EntityList::split(EntityList& left, EntityList& right) const {
  auto half = entities.size() / 2;

  left.entities.reserve(half);
  right.entities.reserve(half);

  std::copy(entities.begin(), entities.begin() + half,
            std::back_inserter(left.entities));
  std::copy(entities.begin() + half, entities.end(),
            std::back_inserter(right.entities));
};

bool EntityList::hit(const math::Ray& ray,
                     const f32 tMin,
                     const f32 tMax,
                     Hit& resultHit) const {
  Hit entityHit;
  f32 tClosest = tMax;
  bool hasHit = false;

  for (auto entity : entities) {
    if (entity->hit(ray, tMin, tClosest, entityHit)) {
      hasHit = true;
      tClosest = entityHit.t;
      resultHit = entityHit;
    }
  }

  return hasHit;
}

bool EntityList::bounds(math::AABB& box) const {
  if (entities.size() == 0)
    return false;

  math::AABB temp;
  bool firstTrue = entities[0]->bounds(temp);

  if (!firstTrue)
    return false;
  else
    box = temp;

  for (auto entity : entities) {
    if (entity->bounds(temp)) {
      box = math::surround(box, temp);
    } else {
      return false;
    }
  }

  return true;
}

bool Sphere::hit(const math::Ray& ray,
                 const f32 tMin,
                 const f32 tMax,
                 Hit& hit) const {
  math::vec3 oc = ray.origin - center;

  f32 a = math::dot(ray.direction, ray.direction);
  f32 b = math::dot(oc, ray.direction);
  f32 c = math::dot(oc, oc) - radius * radius;
  f32 discriminant = b * b - a * c;

  if (discriminant > 0) {
    f32 discSqrt = sqrt(discriminant);
    f32 t = (-b - discSqrt) / a;
    if (t < tMax && t > tMin) {
      hit.t = t;
      hit.p = ray.at(t);
      hit.normal = math::normalize((hit.p - center) / radius);
      hit.material = material;
      sphereTextureCoordinates((hit.p - center) / radius, hit.u, hit.v);
      return true;
    }
    t = (-b + discSqrt) / a;
    if (t < tMax && t > tMin) {
      hit.t = t;
      hit.p = ray.at(t);
      hit.normal = math::normalize((hit.p - center) / radius);
      hit.material = material;
      sphereTextureCoordinates((hit.p - center) / radius, hit.u, hit.v);
      return true;
    }
  }

  return false;
}

bool Sphere::bounds(math::AABB& box) const {
  box = math::AABB(center - math::vec3(radius, radius, radius),
                   center + math::vec3(radius, radius, radius));
  return true;
}

bool Sphere::renderInspector() {
  bool change = false;
  change = ImGui::DragFloat3("center", center.e, 0.05, -1000, 1000) || change;
  change = ImGui::DragFloat("radius", &radius, 0.05, 0.1, 1000) || change;
  return change;
}

bool XYRectangle::hit(const math::Ray& ray,
                      const f32 tMin,
                      const f32 tMax,
                      Hit& hit) const {
  f32 t = (center.z - ray.origin.z) / ray.direction.z;
  if (t < tMin || t > tMax)
    return false;

  f32 x = ray.origin.x + t * ray.direction.x;
  f32 y = ray.origin.y + t * ray.direction.y;

  f32 halfWidth = 0.5f * width;
  f32 halfHeight = 0.5f * height;
  f32 x0 = center.x - halfWidth;
  f32 x1 = center.x + halfWidth;
  f32 y0 = center.y - halfHeight;
  f32 y1 = center.y + halfHeight;

  if (x < x0 || x > x1 || y < y0 || y > y1)
    return false;

  hit.u = (x - x0) / (x1 - x0);
  hit.v = (y - y0) / (y1 - y0);
  hit.t = t;
  hit.p = ray.at(t);
  hit.normal = math::vec3(0, 0, 1);
  hit.material = material;

  return true;
}

bool XYRectangle::bounds(math::AABB& box) const {
  f32 halfWidth = 0.5f * width;
  f32 halfHeight = 0.5f * height;
  box.minPoint = math::vec3(center.x - halfWidth, center.y - halfHeight,
                            center.z - 0.0001f);
  box.maxPoint = math::vec3(center.x + halfWidth, center.y + halfHeight,
                            center.z + 0.0001f);
  return true;
}

bool XYRectangle::renderInspector() {
  bool change = false;
  ImGui::Text("Center:");
  change = ImGui::DragFloat3("##center", center.e, 0.01, -1000, 1000) || change;
  ImGui::Text("Width:");
  change = ImGui::DragFloat("##width", &width, 0.01, 0, 1000) || change;
  ImGui::Text("Height:");
  change = ImGui::DragFloat("##height", &height, 0.01, 0, 1000) || change;
  return change;
}

bool XZRectangle::hit(const math::Ray& ray,
                      const f32 tMin,
                      const f32 tMax,
                      Hit& hit) const {
  f32 t = (center.y - ray.origin.y) / ray.direction.y;
  if (t < tMin || t > tMax)
    return false;

  f32 x = ray.origin.x + t * ray.direction.x;
  f32 z = ray.origin.z + t * ray.direction.z;

  f32 halfWidth = 0.5f * width;
  f32 halfHeight = 0.5f * height;
  f32 x0 = center.x - halfWidth;
  f32 x1 = center.x + halfWidth;
  f32 z0 = center.z - halfHeight;
  f32 z1 = center.z + halfHeight;

  if (x < x0 || x > x1 || z < z0 || z > z1)
    return false;

  hit.u = (x - x0) / (x1 - x0);
  hit.v = (z - z0) / (z1 - z0);
  hit.t = t;
  hit.p = ray.at(t);
  hit.normal = math::vec3(0, 1, 0);
  hit.material = material;

  return true;
}

bool XZRectangle::bounds(math::AABB& box) const {
  f32 halfWidth = 0.5f * width;
  f32 halfHeight = 0.5f * height;
  box.minPoint = math::vec3(center.x - halfWidth, center.y - 0.0001f,
                            center.z - halfHeight);
  box.maxPoint = math::vec3(center.x + halfWidth, center.y + 0.0001f,
                            center.z + halfHeight);
  return true;
}

bool XZRectangle::renderInspector() {
  bool change = false;
  ImGui::Text("Center:");
  change = ImGui::DragFloat3("##center", center.e, 0.01, -1000, 1000) || change;
  ImGui::Text("Width:");
  change = ImGui::DragFloat("##width", &width, 0.01, 0, 1000) || change;
  ImGui::Text("Height:");
  change = ImGui::DragFloat("##height", &height, 0.01, 0, 1000) || change;
  return change;
}

bool YZRectangle::hit(const math::Ray& ray,
                      const f32 tMin,
                      const f32 tMax,
                      Hit& hit) const {
  f32 t = (center.x - ray.origin.x) / ray.direction.x;
  if (t < tMin || t > tMax)
    return false;

  f32 y = ray.origin.y + t * ray.direction.y;
  f32 z = ray.origin.z + t * ray.direction.z;

  f32 halfWidth = 0.5f * width;
  f32 halfHeight = 0.5f * height;
  f32 y0 = center.y - halfWidth;
  f32 y1 = center.y + halfWidth;
  f32 z0 = center.z - halfHeight;
  f32 z1 = center.z + halfHeight;

  if (y < y0 || y > y1 || z < z0 || z > z1)
    return false;

  hit.u = (y - y0) / (y1 - y0);
  hit.v = (z - z0) / (z1 - z0);
  hit.t = t;
  hit.p = ray.at(t);
  hit.normal = math::vec3(1, 0, 0);
  hit.material = material;

  return true;
}

bool YZRectangle::bounds(math::AABB& box) const {
  f32 halfWidth = 0.5f * width;
  f32 halfHeight = 0.5f * height;
  box.minPoint = math::vec3(center.x - 0.0001f, center.y - halfWidth,
                            center.z - halfHeight);
  box.maxPoint = math::vec3(center.x + 0.0001f, center.y + halfWidth,
                            center.z + halfHeight);
  return true;
}

bool YZRectangle::renderInspector() {
  bool change = false;
  ImGui::Text("Center:");
  change = ImGui::DragFloat3("##center", center.e, 0.01, -1000, 1000) || change;
  ImGui::Text("Width:");
  change = ImGui::DragFloat("##width", &width, 0.01, 0, 1000) || change;
  ImGui::Text("Height:");
  change = ImGui::DragFloat("##height", &height, 0.01, 0, 1000) || change;
  return change;
}

void Box::construct() {
  f32 halfWidth = 0.5f * width;
  f32 halfHeight = 0.5f * height;
  f32 halfDepth = 0.5f * depth;
  entities.add(
      new XYRectangle(math::vec3(center.x, center.y, center.z + halfDepth),
                      width, height, material));
  entities.add(new FlipNormals(
      new XYRectangle(math::vec3(center.x, center.y, center.z - halfDepth),
                      width, height, material)));
  entities.add(
      new XZRectangle(math::vec3(center.x, center.y - halfHeight, center.z),
                      width, depth, material));
  entities.add(new FlipNormals(
      new XZRectangle(math::vec3(center.x, center.y + halfHeight, center.z),
                      width, depth, material)));
  entities.add(
      new YZRectangle(math::vec3(center.x + halfWidth, center.y, center.z),
                      height, depth, material));
  entities.add(new FlipNormals(
      new YZRectangle(math::vec3(center.x - halfWidth, center.y, center.z),
                      height, depth, material)));
}

}  // namespace entity
