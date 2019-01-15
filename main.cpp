#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "types.h"
#include "math.h"

u32 imageWidth = 1920;   // 200;   // 480;
u32 imageHeight = 1080;  // 100;  // 270;
u32 samples = 100;       // 200;
u32 maxDepth = 50;       // 100;

// TODO: This is stupid, figure out a better organisation
namespace Material {
struct Material;
}

struct Hit {
  f32 t;
  vec3 p;
  vec3 normal;
  Material::Material* material;
};

struct Ray {
  vec3 origin;
  vec3 direction;

  Ray(){};
  Ray(vec3 origin, vec3 direction) : origin(origin), direction(direction) {}
  vec3 at(f32 t) const { return origin + t * direction; }
};

#include "material.cpp"
#include "entity.cpp"

struct Camera {
  vec3 origin;
  vec3 lowerLeft;
  vec3 horizontal;
  vec3 vertical;
  f32 lensRadius;
  vec3 forward;
  vec3 left;
  vec3 up;

  Camera(const vec3 origin,
         const vec3 lookAt,
         const vec3 worldUp,
         const f32 vFov,
         const f32 aspect,
         const f32 aperture,
         const f32 focusDistance)
      : origin(origin) {
    f32 theta = vFov * M_PI / 180;
    f32 halfHeight = tan(theta / 2);
    f32 halfWidth = aspect * halfHeight;

    forward = normalize(origin - lookAt);
    left = normalize(cross(worldUp, forward));
    up = cross(forward, left);

    lowerLeft = origin - halfWidth * focusDistance * left -
                halfHeight * focusDistance * up - focusDistance * forward;
    horizontal = 2 * halfWidth * focusDistance * left;
    vertical = 2 * halfHeight * focusDistance * up;

    lensRadius = aperture / 2;
  }

  Ray ray(const f32 s, const f32 t) const {
    vec3 rayDirection = lensRadius * randomPointInUnitDisk();
    vec3 offset = left * rayDirection.x + up * rayDirection.y;
    return Ray(origin + offset,
               lowerLeft + s * horizontal + t * vertical - origin - offset);
  }
};

struct World {
  std::vector<Entity::Entity*> entities;

  World() {}

  void addEntity(Entity::Entity* entity) { entities.push_back(entity); }

  bool hit(const Ray& ray, const f32 tMin, const f32 tMax, Hit& hit) const {
    Hit entityHit;
    f32 tClosest = tMax;
    bool hasHit = false;

    for (auto entity : entities) {
      if (entity->hit(ray, tMin, tClosest, entityHit)) {
        hasHit = true;
        tClosest = entityHit.t;
        hit = entityHit;
      }
    }

    return hasHit;
  }
};

World* world = new World();
Camera* camera;

vec3 cast(const World* world, const Ray& ray, u32 depth = 0) {
  Hit hit;

  // Epsilon for ignoring hits around t = 0
  f32 tMin = 0.001f;

  if (world->hit(ray, tMin, FLT_MAX, hit)) {
    Ray scattered;
    vec3 attenuation;

    if (depth < maxDepth &&
        hit.material->scatter(ray, hit, attenuation, scattered)) {
      return attenuation * cast(world, scattered, depth + 1);
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
  world->addEntity(Entity::createSphere(
      vec3(0, -1000, 0), 1000, Material::createDiffuse(vec3(0.5, 0.5, 0.5))));

  world->addEntity(
      Entity::createSphere(vec3(0, 1, 0), 1, Material::createDielectric(1.5)));
  world->addEntity(Entity::createSphere(
      vec3(-3, 1, 0), 1, Material::createDiffuse(vec3(0.4, 0.2, 0.1))));
  world->addEntity(Entity::createSphere(
      vec3(3, 1, 0), 1, Material::createMetal(vec3(0.7, 0.6, 0.5), 1)));

  // world->addEntity(Entity::createSphere(
  //     vec3(0, 0, -1), 0.5f, Material::createDiffuse(vec3(0.1f, 0.2f,
  //     0.5f))));

  // world->addEntity(Entity::createSphere(
  //     vec3(0, -100.5f, -1), 100, Material::createDiffuse(vec3(0.8f, 0.8f,
  //     0))));

  // world->addEntity(Entity::createSphere(
  //     vec3(1, 0, -1), 0.5f, Material::createMetal(vec3(0.8f, 0.6f, 0.2f),
  //     1)));

  // world->addEntity(Entity::createSphere(vec3(-1, 0, -1), 0.5f,
  //                                       Material::createDielectric(1.5f)));

  f32 aspect = f32(imageWidth) / f32(imageHeight);
  vec3 up(0, 1, 0);
  vec3 origin(0, 1, 3);
  vec3 lookAt(0, 0, -1);
  f32 aperture = 0.1;
  f32 focusDistance = 2.5;  //(origin - lookAt).length();
  camera = new Camera(origin, lookAt, up, 60, aspect, aperture, focusDistance);
}

void diffuseDemo() {
  world->addEntity(Entity::createSphere(
      vec3(0, -1000, 0), 1000, Material::createDiffuse(vec3(0.1, 0.1, 0.1))));

  world->addEntity(Entity::createSphere(
      vec3(-2, 1, -1), 1, Material::createDiffuse(vec3(0.5, 0.5, 0.5))));
  world->addEntity(Entity::createSphere(
      vec3(0, 1, -1), 1, Material::createDiffuse(vec3(0.2, 0.45, 0.85))));
  world->addEntity(Entity::createSphere(
      vec3(2, 1, -1), 1, Material::createDiffuse(vec3(0.5, 0.5, 0.5))));

  f32 aspect = f32(imageWidth) / f32(imageHeight);
  vec3 up(0, 1, 0);
  vec3 origin(0, 2, 6);
  vec3 lookAt(0, 1.2, -1);
  f32 aperture = 0.1;
  f32 focusDistance = (origin - lookAt).length();
  camera = new Camera(origin, lookAt, up, 30, aspect, aperture, focusDistance);
}

void metalDemo() {
  world->addEntity(Entity::createSphere(
      vec3(0, -1000, 0), 1000, Material::createDiffuse(vec3(0.1, 0.1, 0.1))));

  world->addEntity(Entity::createSphere(
      vec3(-2, 1, -1), 1, Material::createMetal(vec3(0.5, 0.5, 0.5), 0)));
  world->addEntity(Entity::createSphere(
      vec3(0, 1, -1), 1, Material::createDiffuse(vec3(0.2, 0.45, 0.85))));
  world->addEntity(Entity::createSphere(
      vec3(2, 1, -1), 1, Material::createMetal(vec3(0.5, 0.5, 0.5), 0)));

  f32 aspect = f32(imageWidth) / f32(imageHeight);
  vec3 up(0, 1, 0);
  vec3 origin(0, 2, 6);
  vec3 lookAt(0, 1.2, -1);
  f32 aperture = 0.1;
  f32 focusDistance = (origin - lookAt).length();
  camera = new Camera(origin, lookAt, up, 30, aspect, aperture, focusDistance);
}

void glassDemo() {
  world->addEntity(Entity::createSphere(
      vec3(0, -1000, 0), 1000, Material::createDiffuse(vec3(0.1, 0.1, 0.1))));

  world->addEntity(Entity::createSphere(
      vec3(-2, 1, -1), 1, Material::createDiffuse(vec3(0.5, 0.5, 0.5))));
  world->addEntity(
      Entity::createSphere(vec3(0, 1, -1), 1, Material::createDielectric(1.5)));
  world->addEntity(Entity::createSphere(
      vec3(2, 1, -1), 1, Material::createDiffuse(vec3(0.5, 0.5, 0.5))));

  f32 aspect = f32(imageWidth) / f32(imageHeight);
  vec3 up(0, 1, 0);
  vec3 origin(0, 2, 6);
  vec3 lookAt(0, 1.2, -1);
  f32 aperture = 0.1;
  f32 focusDistance = (origin - lookAt).length();
  camera = new Camera(origin, lookAt, up, 30, aspect, aperture, focusDistance);
}

void spheresWorld() {
  world->addEntity(Entity::createSphere(
      vec3(0, -1000, 0), 1000, Material::createDiffuse(vec3(0.5, 0.5, 0.5))));

  for (s32 a = -11; a < 11; a++) {
    for (s32 b = -11; b < 11; b++) {
      f32 chooseMat = drand48();
      vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        if (chooseMat < 0.8) {
          world->addEntity(Entity::createSphere(
              center, 0.2,
              Material::createDiffuse(vec3(drand48() * drand48(),
                                           drand48() * drand48(),
                                           drand48() * drand48()))));

        } else if (chooseMat < 0.90) {
          world->addEntity(Entity::createSphere(
              center, 0.2,
              Material::createMetal(
                  vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()),
                       0.5 * (1 + drand48())),
                  1 - (0.5 * drand48()))));

        } else {
          world->addEntity(Entity::createSphere(
              center, 0.2, Material::createDielectric(1.5)));
        }
      }
    }
  }

  world->addEntity(
      Entity::createSphere(vec3(0, 1, 0), 1, Material::createDielectric(1.5)));
  world->addEntity(Entity::createSphere(
      vec3(-4, 1, 0), 1, Material::createDiffuse(vec3(0.4, 0.2, 0.1))));
  world->addEntity(Entity::createSphere(
      vec3(4, 1, 0), 1, Material::createMetal(vec3(0.7, 0.6, 0.5), 1)));

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
  camera = new Camera(origin, lookAt, up, 20, aspect, aperture, focusDistance);
}

s32 main() {
  spheresWorld();
  // testWorld();
  // diffuseDemo();
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

        Ray ray = camera->ray(u, v);
        color += cast(world, ray);
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