namespace entity {

const char* toString(EntityType type) {
  switch (type) {
    case EntityType::Sphere:
      return "Sphere";
    default:
      assert("Unknown entity type");
  };
  return nullptr;
}

std::string toString(const math::vec3& v) {
  std::ostringstream ss;
  ss << "[" << v.x << ", " << v.y << ", " << v.z << "]";
  return ss.str();
}

std::string toString(const Sphere* entity) {
  std::ostringstream ss;
  ss << "Sphere " << toString(entity->center);
  return ss.str();
}

const char* toString(const Entity* entity) {
  switch (entity->type()) {
    case EntityType::Sphere:
      return toString((const Sphere*)entity).c_str();
    default:
      assert("Unknown entity type");
  };
  return nullptr;
}

Entity* createEntity(EntityType type) {
  switch (type) {
    case EntityType::Sphere:
      return new Sphere({0, 0, 0}, 1);
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
    f32 t = (-b - sqrt(discriminant)) / a;
    if (t < tMax && t > tMin) {
      hit.t = t;
      hit.p = ray.at(t);
      hit.normal = math::normalize((hit.p - center) / radius);
      hit.material = material;
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

}  // namespace entity
