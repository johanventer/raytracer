void testDemo() {
  worldEntities.add(new entity::Sphere(
      {0, 0, 0}, 2,
      new material::Diffuse(new texture::Solid(math::vec3(1, 0, 0)))));
  worldEntities.add(new entity::Sphere(
      {4, 0, 0}, 2,
      new material::Diffuse(new texture::Solid(math::vec3(0, 1, 0)))));
  worldEntities.add(new entity::Sphere(
      {-4, 0, 0}, 2,
      new material::Diffuse(new texture::Solid(math::vec3(0, 0, 1)))));
  worldEntities.add(new entity::Sphere(
      {0, -4, 0}, 2,
      new material::Diffuse(new texture::Solid(math::vec3(0.5, 0.5, 0.5)))));
  worldEntities.add(new entity::Sphere(
      {0, 4, 0}, 2,
      new material::Diffuse(new texture::Solid(math::vec3(0.8, 0.3, 0.3)))));
}

void simpleDemo() {
  worldEntities.add(new entity::Sphere(
      {0, -1000, 0}, 1000,
      new material::Diffuse(new texture::Solid(math::vec3(0.5, 0.5, 0.5)))));

  worldEntities.add(new entity::Sphere(math::vec3(0, 1, 0), 1,
                                       new material::Dielectric(1.5)));
  worldEntities.add(new entity::Sphere(
      {-3, 1, 0}, 1,
      new material::Diffuse(new texture::Solid(math::vec3(0.4, 0.2, 0.1)))));
  worldEntities.add(new entity::Sphere(
      {3, 1, 0}, 1,
      new material::Metal(new texture::Solid(math::vec3(0.7, 0.6, 0.5)), 1)));

  // vec3 up{0, 1, 0};
  // vec3 origin{0, 1, 3};
  // vec3 lookAt{0, 0, -1};
  // f32 aperture = 0.1;
  // f32 focusDistance = 2.5;  //(origin - lookAt).length();
  // mainCamera = camera::createCamera(origin, lookAt, up, screenWidth,
  //                                   screenHeight, 60, aperture,
  //                                   focusDistance);
}

void diffuseDemo() {
  worldEntities.add(new entity::Sphere(
      math::vec3(0, -1000, 0), 1000,
      new material::Diffuse(new texture::Checker(30, math::vec3(0.1, 0.1, 0.1),
                                                 math::vec3(0.9, 0.9, 0.9)))));

  worldEntities.add(new entity::Sphere(
      math::vec3(-2, 1, -1), 1,
      new material::Diffuse(new texture::Solid(math::vec3(0.5, 0.5, 0.5)))));
  worldEntities.add(new entity::Sphere(
      math::vec3(0, 1, -1), 1,
      new material::Diffuse(new texture::Solid(math::vec3(0.2, 0.45, 0.85)))));
  worldEntities.add(new entity::Sphere(
      math::vec3(2, 1, -1), 1,
      new material::Diffuse(new texture::Solid(math::vec3(0.5, 0.5, 0.5)))));

  // vec3 up{0, 1, 0};
  // vec3 origin{0, 2, 6};
  // vec3 lookAt{0, 1.2, -1};
  // f32 aperture = 0.1;
  // f32 focusDistance = (origin - lookAt).length();
  // mainCamera = camera::createCamera(origin, lookAt, up, screenWidth,
  //                                   screenHeight, 30, aperture,
  //                                   focusDistance);
}

void metalDemo() {
  worldEntities.add(new entity::Sphere(
      math::vec3(0, -1000, 0), 1000,
      new material::Diffuse(new texture::Solid(math::vec3(0.1, 0.1, 0.1)))));

  worldEntities.add(new entity::Sphere(
      math::vec3(-2, 1, -1), 1,
      new material::Metal(new texture::Solid(math::vec3(0.5, 0.5, 0.5)), 1)));
  worldEntities.add(new entity::Sphere(
      math::vec3(0, 1, -1), 1,
      new material::Diffuse(new texture::Solid(math::vec3(0.2, 0.45, 0.85)))));
  worldEntities.add(new entity::Sphere(
      math::vec3(2, 1, -1), 1,
      new material::Metal(new texture::Solid(math::vec3(0.5, 0.5, 0.5)), 0.3)));

  // vec3 up{0, 1, 0};
  // vec3 origin{0, 2, 6};
  // vec3 lookAt{0, 1.2, -1};
  // f32 aperture = 0.1;
  // f32 focusDistance = (origin - lookAt).length();
  // mainCamera = camera::createCamera(origin, lookAt, up, screenWidth,
  //                                   screenHeight, 30, aperture,
  //                                   focusDistance);
}

void glassDemo() {
  worldEntities.add(new entity::Sphere(
      math::vec3(0, -1000, 0), 1000,
      new material::Diffuse(new texture::Solid(math::vec3(0.1, 0.1, 0.1)))));

  worldEntities.add(new entity::Sphere(
      math::vec3(-2, 1, -1), 1,
      new material::Diffuse(new texture::Solid(math::vec3(0.5, 0.5, 0.5)))));
  worldEntities.add(new entity::Sphere(math::vec3(0, 1, -1), 1,
                                       new material::Dielectric(1.5)));
  worldEntities.add(new entity::Sphere(
      math::vec3(2, 1, -1), 1,
      new material::Diffuse(new texture::Solid(math::vec3(0.5, 0.5, 0.5)))));

  // vec3 up{0, 1, 0};
  // vec3 origin{0, 2, 6};
  // vec3 lookAt{0, 1.2, -1};
  // f32 aperture = 0.1;
  // f32 focusDistance = (origin - lookAt).length();
  // mainCamera = camera::createCamera(origin, lookAt, up, screenWidth,
  //                                   screenHeight, 30, aperture,
  //                                   focusDistance);
}

void spheresWorld() {
  worldEntities.add(new entity::Sphere(
      math::vec3(0, -1000, 0), 1000,
      new material::Diffuse(new texture::Solid(math::vec3(0.5, 0.5, 0.5)))));

  for (s32 a = -11; a < 11; a++) {
    for (s32 b = -11; b < 11; b++) {
      f32 chooseMat = math::rand01();
      math::vec3 center(a + 0.9f * math::rand01(), 0.2f,
                        b + 0.9f * math::rand01());
      if (math::length(center - math::vec3(4, 0.2, 0)) > 0.9) {
        if (chooseMat < 0.8) {
          worldEntities.add(new entity::Sphere(
              center, 0.2,
              new material::Diffuse(new texture::Solid(
                  math::vec3(math::rand01() * math::rand01(),
                             math::rand01() * math::rand01(),
                             math::rand01() * math::rand01())))));

        } else if (chooseMat < 0.90) {
          worldEntities.add(new entity::Sphere(
              center, 0.2,
              new material::Metal(
                  new texture::Solid(math::vec3(0.5f * (1 + math::rand01()),
                                                0.5f * (1 + math::rand01()),
                                                0.5f * (1 + math::rand01()))),
                  1 - (0.5 * math::rand01()))));

        } else {
          worldEntities.add(
              new entity::Sphere(center, 0.2, new material::Dielectric(1.5)));
        }
      }
    }
  }

  worldEntities.add(new entity::Sphere(math::vec3(0, 1, 0), 1,
                                       new material::Dielectric(1.5)));
  worldEntities.add(new entity::Sphere(
      math::vec3(-4, 1, 0), 1,
      new material::Diffuse(new texture::Solid(math::vec3(0.4, 0.2, 0.1)))));
  worldEntities.add(new entity::Sphere(
      math::vec3(4, 1, 0), 1,
      new material::Metal(new texture::Solid(math::vec3(0.7, 0.6, 0.5)), 1)));

  mainCamera.reset(
      new camera::Camera(screenWidth, screenHeight, 10, 20, 0, 1, -15, 0));

  mainCamera->update(0);
}
