void addEntity(EntityList& entities, entity::Entity* entity) {
  entities.push_back(entity);
}

bool findHit(const EntityList& entities,
             const camera::Ray& ray,
             const f32 tMin,
             const f32 tMax,
             Hit& hit) {
  Hit entityHit;
  f32 tClosest = tMax;
  bool hasHit = false;

  for (auto entity : entities) {
    if (entity::findHit(entity, ray, tMin, tClosest, entityHit)) {
      hasHit = true;
      tClosest = entityHit.t;
      hit = entityHit;
    }
  }

  return hasHit;
}

bool getBoundingBox(const EntityList entities, bvh::AABB& box) {
  if (entities.size() == 0)
    return false;

  bvh::AABB temp;
  bool firstTrue = entity::getBoundingBox(entities[0], temp);

  if (!firstTrue)
    return false;
  else
    box = temp;

  for (auto entity : entities) {
    if (entity::getBoundingBox(entity, temp)) {
      box = bvh::surroundingBox(box, temp);
    } else {
      return false;
    }
  }

  return true;
}
