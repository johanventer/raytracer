namespace bvh {

BoundingVolume::BoundingVolume(entity::EntityList& allEntities) {
  left = nullptr;
  right = nullptr;

  if (allEntities.size() == 0) {
    return;
  }

  u32 axis = u32(3 * math::rand01());
  allEntities.sort(axis);

  if (allEntities.size() == 1) {
    entities = allEntities;
    if (!allEntities.bounds(box)) {
      fatal("Failed to get bounding box");
    }
  } else {
    entity::EntityList leftEntities;
    entity::EntityList rightEntities;
    allEntities.split(leftEntities, rightEntities);

    left = new BoundingVolume(leftEntities);
    right = new BoundingVolume(rightEntities);

    math::AABB leftBox, rightBox;
    if (!leftEntities.bounds(leftBox) || !rightEntities.bounds(rightBox)) {
      fatal("Failed to get both bounding boxes");
    }

    box = math::surround(leftBox, rightBox);
  }
}

bool BoundingVolume::hit(const math::Ray& ray,
                         f32 tMin,
                         f32 tMax,
                         Hit& resultHit) const {
  if (box.hit(ray, tMin, tMax)) {
    Hit leftHit, rightHit;
    bool hitLeft = false;
    bool hitRight = false;

    if (left)
      hitLeft = left->hit(ray, tMin, tMax, leftHit);
    if (right)
      hitRight = right->hit(ray, tMin, tMax, rightHit);

    if (hitLeft && hitRight) {
      resultHit = leftHit.t < rightHit.t ? leftHit : rightHit;
      return true;

    } else if (hitLeft) {
      resultHit = leftHit;
      return true;

    } else if (hitRight) {
      resultHit = rightHit;
      return true;

    } else {
      return entities.hit(ray, tMin, tMax, resultHit);
    }
  } else {
    return false;
  }
}

void printBvh(bvh::BoundingVolume* bvh, u32 depth = 0) {
  auto spacer = std::string(depth * 4, ' ');
  std::cerr << spacer << bvh->entities.size() << " entities ["
            << bvh->box.minPoint << ", " << bvh->box.maxPoint << "]\n";
  if (bvh->left) {
    printBvh(bvh->left, depth + 1);
  }
  if (bvh->right) {
    printBvh(bvh->right, depth + 1);
  }
}

};  // namespace bvh