namespace bvh {

// TODO: Upgrade to Andrew Kensler's faster code
bool findHit(const AABB& aabb, const camera::Ray& ray, f32 tMin, f32 tMax) {
  for (u32 axis = 0; axis < 3; axis++) {
    f32 t0 =
        min((aabb.minPoint[axis] - ray.origin[axis]) / ray.direction[axis],
            (aabb.maxPoint[axis] - ray.origin[axis]) / ray.direction[axis]);
    f32 t1 =
        max((aabb.minPoint[axis] - ray.origin[axis]) / ray.direction[axis],
            (aabb.maxPoint[axis] - ray.origin[axis]) / ray.direction[axis]);

    tMin = max(t0, tMin);
    tMax = min(t1, tMax);

    if (tMax <= tMin)
      return false;
  }
  return true;
}

AABB createAABB(const vec3& minPoint, const vec3& maxPoint) {
  AABB aabb;
  aabb.minPoint = minPoint;
  aabb.maxPoint = maxPoint;
  return aabb;
}

AABB surroundingBox(const AABB& box0, const AABB& box1) {
  vec3 minPoint(min(box0.minPoint.x, box1.minPoint.x),
                min(box0.minPoint.y, box1.minPoint.y),
                min(box0.minPoint.z, box1.minPoint.z));
  vec3 maxPoint(max(box0.maxPoint.x, box1.maxPoint.x),
                max(box0.maxPoint.y, box1.maxPoint.y),
                max(box0.maxPoint.z, box1.maxPoint.z));
  return createAABB(minPoint, maxPoint);
}

bool findHit(const BoundingVolume* volume,
             const camera::Ray& ray,
             f32 tMin,
             f32 tMax,
             Hit& hit,
             u32 depth = 0) {
  if (findHit(volume->box, ray, tMin, tMax)) {
    Hit leftHit, rightHit;
    bool hitLeft = false;
    bool hitRight = false;

    if (volume->left)
      hitLeft = findHit(volume->left, ray, tMin, tMax, leftHit, depth + 1);
    if (volume->right)
      hitRight = findHit(volume->right, ray, tMin, tMax, rightHit, depth + 1);

    if (hitLeft && hitRight) {
      hit = leftHit.t < rightHit.t ? leftHit : rightHit;
      return true;

    } else if (hitLeft) {
      hit = leftHit;
      return true;

    } else if (hitRight) {
      hit = rightHit;
      return true;

    } else {
      return findHit(volume->entities, ray, tMin, tMax, hit);
    }
  } else {
    return false;
  }
}

BoundingVolume::BoundingVolume(EntityList& _entities, u32 depth = 0) {
  // auto spacer = std::string(depth * 8, ' ');
  // std::cout << spacer << "BoundingVolume: " << _entities.size() << "\n";

  left = nullptr;
  right = nullptr;

  if (_entities.size() == 0) {
    return;
  }

  u32 axis = u32(3 * drand48());

  auto sortEntities = [](const entity::Entity* a, const entity::Entity* b,
                         u32 axis) {
    AABB aBox, bBox;
    if (!entity::getBoundingBox(a, aBox) || !entity::getBoundingBox(b, bBox)) {
      fatal("No bounding boxes during comparison");
    }
    return (aBox.minPoint[axis] - bBox.minPoint[axis]) < 0.0;
  };

  std::sort(std::begin(_entities), std::end(_entities),
            [axis, sortEntities](entity::Entity* a, entity::Entity* b) {
              return sortEntities(a, b, axis);
            });

  if (_entities.size() == 1) {
    entities = _entities;
    if (!getBoundingBox(_entities, box)) {
      fatal("Failed to get bounding box");
    }
  } else {
    u32 half = _entities.size() / 2;

    EntityList leftEntities;
    EntityList rightEntities;
    std::copy(_entities.begin(), _entities.begin() + half,
              std::back_inserter(leftEntities));
    std::copy(_entities.begin() + half, _entities.end(),
              std::back_inserter(rightEntities));

    left = new BoundingVolume(leftEntities);
    right = new BoundingVolume(rightEntities);

    AABB leftBox, rightBox;
    if (!getBoundingBox(leftEntities, leftBox) ||
        !getBoundingBox(rightEntities, rightBox)) {
      fatal("Failed to get both bounding boxes");
    }

    box = surroundingBox(leftBox, rightBox);
  }
}

};  // namespace bvh