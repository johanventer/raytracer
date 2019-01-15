#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "types.h"
#include "math.h"
#include "camera.h"
#include "material.h"
#include "entity.h"

struct Hit {
  f32 t;
  vec3 p;
  vec3 normal;
  material::Material* material;
};

// struct BoundingVolume {
//   AABB box;
//   World* left;
//   World* right;

//   BoundingVolume() {}
//   BoundingVolume(World* world);

//   bool hit(const Ray& ray, f32 tMin, f32 tMax, Hit& hit) const;
//   bool aabb(AABB& box) const;
// };

// BoundingVolume::BoundingVolume(World* world) {
//   u32 axis = u32(3 * drand48());
//   if (axis == 0) {
//   } else if (axis == 1) {
//   } else {
//   }
// }

// bool BoundingVolume::aabb(AABB& b) const {
//   b = box;
// }

// bool BoundingVolume::hit(const Ray& ray, f32 tMin, f32 tMax, Hit& hit) const
// {
//   if (box.hit(ray, tMin, tMax)) {
//     Hit leftHit, rightHit;
//     bool hitLeft = left->hit(ray, tMin, tMax, leftHit);
//     bool hitRight = right->hit(ray, tMin, tMax, rightHit);
//     if (hitLeft && hitRight) {
//       if (leftHit.t < rightHit.t) {
//         hit = leftHit;
//       } else {
//         hit = rightHit;
//       }
//       return true
//     } else if (hitLeft) {
//       hit = leftHit;
//       return true;
//     } else if (hitRight) {
//       hit = rightHit;
//       return true;
//     } else {
//       return false;
//     }
//   } else {
//     return false;
//   }
// }

#include "camera.cpp"
#include "material.cpp"
#include "entity.cpp"

u32 imageWidth = 200;   // 480;
u32 imageHeight = 100;  // 270;
u32 samples = 100;      // 200;
u32 maxDepth = 50;      // 100;

typedef std::vector<entity::Entity*> EntityList;

void addEntity(EntityList& entities, entity::Entity* entity) {
  entities.push_back(entity);
}

bool findHit(const EntityList& entities,
             const camera::Ray& ray,
             const f32 tMin,
             const f32 tMax,
             Hit& hitResult) {
  Hit entityHit;
  f32 tClosest = tMax;
  bool hasHit = false;

  for (auto entity : entities) {
    if (entity::hit(entity, ray, tMin, tClosest, entityHit)) {
      hasHit = true;
      tClosest = entityHit.t;
      hitResult = entityHit;
    }
  }

  return hasHit;
}

// bool boundingBox(const EntityList entities, AABB& box) const {
//   if (entities.size() < 1)
//     return false;

//   AABB temp;
//   bool firstTrue = entity::boundingBox(entities[0], temp);
//   if (firstTrue)
//     return false;
//   else
//     box = temp;

//   for (auto entity : entities) {
//     if (entity::boundingBox(entity, temp)) {
//       box = surroundingBox(box, temp);
//     } else {
//       return false;
//     }
//   }

//   return true;
// }

camera::Camera* mainCamera;
EntityList worldEntities;

vec3 cast(const EntityList& entities, const camera::Ray& ray, u32 depth = 0) {
  Hit hitResult;

  // Epsilon for ignoring hits around t = 0
  f32 tMin = 0.001f;

  if (findHit(entities, ray, tMin, FLT_MAX, hitResult)) {
    camera::Ray scattered;
    vec3 attenuation;

    if (depth < maxDepth &&
        material::scatter(hitResult.material, ray, hitResult, attenuation,
                          scattered)) {
      return attenuation * cast(entities, scattered, depth + 1);
    } else {
      return vec3(0, 0, 0);
    }

    // Visualise normals
    // return 0.5f * vec3(hit.normal.x + 1, hit.normal.y + 1, hit.normal.z +
    // 1);
  }

  vec3 unit_direction = normalize(ray.direction);
  f32 t = 0.5f * (unit_direction.y + 1);
  return lerp(vec3(1, 1, 1), vec3(0.5, 0.7, 1), t);
}

void testWorld() {
  addEntity(worldEntities,
            entity::createSphere(vec3(0, -1000, 0), 1000,
                                 material::createDiffuse(vec3(0.5, 0.5, 0.5))));

  addEntity(
      worldEntities,
      entity::createSphere(vec3(0, 1, 0), 1, material::createDielectric(1.5)));
  addEntity(worldEntities,
            entity::createSphere(vec3(-3, 1, 0), 1,
                                 material::createDiffuse(vec3(0.4, 0.2, 0.1))));
  addEntity(worldEntities, entity::createSphere(
                               vec3(3, 1, 0), 1,
                               material::createMetal(vec3(0.7, 0.6, 0.5), 1)));

  // addEntity(worldEntities, entity::createSphere(
  //     vec3(0, 0, -1), 0.5f, material::createDiffuse(vec3(0.1f, 0.2f,
  //     0.5f))));

  // addEntity(worldEntities, entity::createSphere(
  //     vec3(0, -100.5f, -1), 100, material::createDiffuse(vec3(0.8f, 0.8f,
  //     0))));

  // addEntity(worldEntities, entity::createSphere(
  //     vec3(1, 0, -1), 0.5f, material::createMetal(vec3(0.8f, 0.6f, 0.2f),
  //     1)));

  // addEntity(worldEntities, entity::createSphere(vec3(-1, 0, -1), 0.5f,
  //                                       material::createDielectric(1.5f)));

  f32 aspect = f32(imageWidth) / f32(imageHeight);
  vec3 up(0, 1, 0);
  vec3 origin(0, 1, 3);
  vec3 lookAt(0, 0, -1);
  f32 aperture = 0.1;
  f32 focusDistance = 2.5;  //(origin - lookAt).length();
  mainCamera = camera::createCamera(origin, lookAt, up, 60, aspect, aperture,
                                    focusDistance);
}

void diffuseDemo() {
  addEntity(worldEntities,
            entity::createSphere(vec3(0, -1000, 0), 1000,
                                 material::createDiffuse(vec3(0.1, 0.1, 0.1))));

  addEntity(worldEntities,
            entity::createSphere(vec3(-2, 1, -1), 1,
                                 material::createDiffuse(vec3(0.5, 0.5, 0.5))));
  addEntity(worldEntities, entity::createSphere(
                               vec3(0, 1, -1), 1,
                               material::createDiffuse(vec3(0.2, 0.45, 0.85))));
  addEntity(worldEntities,
            entity::createSphere(vec3(2, 1, -1), 1,
                                 material::createDiffuse(vec3(0.5, 0.5, 0.5))));

  f32 aspect = f32(imageWidth) / f32(imageHeight);
  vec3 up(0, 1, 0);
  vec3 origin(0, 2, 6);
  vec3 lookAt(0, 1.2, -1);
  f32 aperture = 0.1;
  f32 focusDistance = (origin - lookAt).length();
  mainCamera = camera::createCamera(origin, lookAt, up, 30, aspect, aperture,
                                    focusDistance);
}

void metalDemo() {
  addEntity(worldEntities,
            entity::createSphere(vec3(0, -1000, 0), 1000,
                                 material::createDiffuse(vec3(0.1, 0.1, 0.1))));

  addEntity(worldEntities, entity::createSphere(
                               vec3(-2, 1, -1), 1,
                               material::createMetal(vec3(0.5, 0.5, 0.5), 0)));
  addEntity(worldEntities, entity::createSphere(
                               vec3(0, 1, -1), 1,
                               material::createDiffuse(vec3(0.2, 0.45, 0.85))));
  addEntity(worldEntities, entity::createSphere(
                               vec3(2, 1, -1), 1,
                               material::createMetal(vec3(0.5, 0.5, 0.5), 0)));

  f32 aspect = f32(imageWidth) / f32(imageHeight);
  vec3 up(0, 1, 0);
  vec3 origin(0, 2, 6);
  vec3 lookAt(0, 1.2, -1);
  f32 aperture = 0.1;
  f32 focusDistance = (origin - lookAt).length();
  mainCamera = camera::createCamera(origin, lookAt, up, 30, aspect, aperture,
                                    focusDistance);
}

void glassDemo() {
  addEntity(worldEntities,
            entity::createSphere(vec3(0, -1000, 0), 1000,
                                 material::createDiffuse(vec3(0.1, 0.1, 0.1))));

  addEntity(worldEntities,
            entity::createSphere(vec3(-2, 1, -1), 1,
                                 material::createDiffuse(vec3(0.5, 0.5, 0.5))));
  addEntity(
      worldEntities,
      entity::createSphere(vec3(0, 1, -1), 1, material::createDielectric(1.5)));
  addEntity(worldEntities,
            entity::createSphere(vec3(2, 1, -1), 1,
                                 material::createDiffuse(vec3(0.5, 0.5, 0.5))));

  f32 aspect = f32(imageWidth) / f32(imageHeight);
  vec3 up(0, 1, 0);
  vec3 origin(0, 2, 6);
  vec3 lookAt(0, 1.2, -1);
  f32 aperture = 0.1;
  f32 focusDistance = (origin - lookAt).length();
  mainCamera = camera::createCamera(origin, lookAt, up, 30, aspect, aperture,
                                    focusDistance);
}

void spheresWorld() {
  addEntity(worldEntities,
            entity::createSphere(vec3(0, -1000, 0), 1000,
                                 material::createDiffuse(vec3(0.5, 0.5, 0.5))));

  for (s32 a = -11; a < 11; a++) {
    for (s32 b = -11; b < 11; b++) {
      f32 chooseMat = drand48();
      vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        if (chooseMat < 0.8) {
          addEntity(worldEntities,
                    entity::createSphere(
                        center, 0.2,
                        material::createDiffuse(vec3(drand48() * drand48(),
                                                     drand48() * drand48(),
                                                     drand48() * drand48()))));

        } else if (chooseMat < 0.90) {
          addEntity(worldEntities,
                    entity::createSphere(
                        center, 0.2,
                        material::createMetal(
                            vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()),
                                 0.5 * (1 + drand48())),
                            1 - (0.5 * drand48()))));

        } else {
          addEntity(worldEntities,
                    entity::createSphere(center, 0.2,
                                         material::createDielectric(1.5)));
        }
      }
    }
  }

  addEntity(
      worldEntities,
      entity::createSphere(vec3(0, 1, 0), 1, material::createDielectric(1.5)));
  addEntity(worldEntities,
            entity::createSphere(vec3(-4, 1, 0), 1,
                                 material::createDiffuse(vec3(0.4, 0.2, 0.1))));
  addEntity(worldEntities, entity::createSphere(
                               vec3(4, 1, 0), 1,
                               material::createMetal(vec3(0.7, 0.6, 0.5), 1)));

  f32 aspect = f32(imageWidth) / f32(imageHeight);
  vec3 up(0, 1, 0);
  // vec3 origin(5.0, 1.5, 2.0);
  // vec3 lookAt(0, 0, -1);
  // f32 aperture = 0.1;
  // f32 focusDistance = 4;  //(origin - lookAt).length();
  vec3 origin(13, 2, 3);
  vec3 lookAt(0, 0, 0);
  f32 aperture = 0.0;
  f32 focusDistance = 10;  //(origin - lookAt).length();
  mainCamera = camera::createCamera(origin, lookAt, up, 20, aspect, aperture,
                                    focusDistance);
}

s32 main() {
  // spheresWorld();
  // testWorld();
  diffuseDemo();
  // metalDemo();
  // glassDemo();

  std::ofstream outfile("test.ppm", std::ios_base::out);

  outfile << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

  for (s32 y = imageHeight - 1; y >= 0; y--) {
    f32 percent = ((imageHeight - y) / f32(imageHeight)) * 100.0f;
    std::cout << percent << "%\n";

    for (s32 x = 0; x < imageWidth; x++) {
      vec3 color(0, 0, 0);

      // Cast rays, collecting samples
      for (s32 sampleIndex = 0; sampleIndex < samples; sampleIndex++) {
        f32 u = f32(x + drand48()) / f32(imageWidth);
        f32 v = f32(y + drand48()) / f32(imageHeight);

        camera::Ray r = camera::ray(mainCamera, u, v);
        color += cast(worldEntities, r);
      }

      // Blend samples (anti-aliasing)
      color /= f32(samples);

      // Gamma correct (gamma 2 for now)
      color = vec3(sqrt(color.r), sqrt(color.g), sqrt(color.b));

      u32 ir = u32(255.99 * color.r);
      u32 ib = u32(255.99 * color.b);
      u32 ig = u32(255.99 * color.g);

      outfile << ir << " " << ig << " " << ib << "\n";
    }
  }
}