namespace bvh {

// TODO: Upgrade to Andrew Kensler's faster code
bool findHit(AABB& aabb, const camera::Ray& ray, f32 tMin, f32 tMax) {
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

AABB surroundingBox(const AABB& box0, const AABB& box1) {
  vec3 minPoint(min(box0.minPoint.x, box1.minPoint.x),
                min(box0.minPoint.y, box1.minPoint.y),
                min(box0.minPoint.z, box1.minPoint.z));
  vec3 maxPoint(max(box0.maxPoint.x, box1.maxPoint.x),
                max(box0.maxPoint.y, box1.maxPoint.y),
                max(box0.maxPoint.z, box1.maxPoint.z));
  return createAABB(minPoint, maxPoint);
}

AABB createAABB(vec3 minPoint, vec3 maxPoint) {
  AABB aabb;
  aabb.minPoint = minPoint;
  aabb.maxPoint = maxPoint;
  return aabb;
}

bool findHit(BoundingVolume& volume,
             const camera::Ray& ray,
             f32 tMin,
             f32 tMax,
             Hit& hit) {
  if (findHit(volume.box, ray, tMin, tMax)) {
    Hit leftHit, rightHit;
    bool hitLeft = findHit(volume.left, ray, tMin, tMax, leftHit);
    bool hitRight = findHit(volume.right, ray, tMin, tMax, rightHit);
    if (hitLeft && hitRight) {
      if (leftHit.t < rightHit.t) {
        hit = leftHit;
      } else {
        hit = rightHit;
      }
      return true;
    } else if (hitLeft) {
      hit = leftHit;
      return true;
    } else if (hitRight) {
      hit = rightHit;
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

};  // namespace bvh