void testDemo() {
  addEntity(
      worldEntities,
      entity::createSphere({0, 0, 0}, 2, material::createDiffuse({1, 0, 0})));
  addEntity(
      worldEntities,
      entity::createSphere({4, 0, 0}, 2, material::createDiffuse({0, 1, 0})));
  addEntity(
      worldEntities,
      entity::createSphere({-4, 0, 0}, 2, material::createDiffuse({0, 0, 1})));
  addEntity(worldEntities,
            entity::createSphere({0, -4, 0}, 2,
                                 material::createDiffuse({0.5, 0.5, 0.5})));
  addEntity(worldEntities,
            entity::createSphere({0, 4, 0}, 2,
                                 material::createDiffuse({0.8, 0.3, 0.3})));
}

void simpleDemo() {
  addEntity(worldEntities,
            entity::createSphere({0, -1000, 0}, 1000,
                                 material::createDiffuse({0.5, 0.5, 0.5})));

  addEntity(worldEntities, entity::createSphere(
                               {0, 1, 0}, 1, material::createDielectric(1.5)));
  addEntity(worldEntities,
            entity::createSphere({-3, 1, 0}, 1,
                                 material::createDiffuse({0.4, 0.2, 0.1})));
  addEntity(worldEntities,
            entity::createSphere({3, 1, 0}, 1,
                                 material::createMetal({0.7, 0.6, 0.5}, 1)));

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
  addEntity(worldEntities,
            entity::createSphere({0, -1000, 0}, 1000,
                                 material::createDiffuse({0.1, 0.1, 0.1})));

  addEntity(worldEntities,
            entity::createSphere({-2, 1, -1}, 1,
                                 material::createDiffuse({0.5, 0.5, 0.5})));
  addEntity(worldEntities,
            entity::createSphere({0, 1, -1}, 1,
                                 material::createDiffuse({0.2, 0.45, 0.85})));
  addEntity(worldEntities,
            entity::createSphere({2, 1, -1}, 1,
                                 material::createDiffuse({0.5, 0.5, 0.5})));

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
  addEntity(worldEntities,
            entity::createSphere({0, -1000, 0}, 1000,
                                 material::createDiffuse({0.1, 0.1, 0.1})));

  addEntity(worldEntities,
            entity::createSphere({-2, 1, -1}, 1,
                                 material::createMetal({0.5, 0.5, 0.5}, 1)));
  addEntity(worldEntities,
            entity::createSphere({0, 1, -1}, 1,
                                 material::createDiffuse({0.2, 0.45, 0.85})));
  addEntity(worldEntities,
            entity::createSphere({2, 1, -1}, 1,
                                 material::createMetal({0.5, 0.5, 0.5}, 0.3)));

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
  addEntity(worldEntities,
            entity::createSphere({0, -1000, 0}, 1000,
                                 material::createDiffuse({0.1, 0.1, 0.1})));

  addEntity(worldEntities,
            entity::createSphere({-2, 1, -1}, 1,
                                 material::createDiffuse({0.5, 0.5, 0.5})));
  addEntity(worldEntities, entity::createSphere(
                               {0, 1, -1}, 1, material::createDielectric(1.5)));
  addEntity(worldEntities,
            entity::createSphere({2, 1, -1}, 1,
                                 material::createDiffuse({0.5, 0.5, 0.5})));

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
  addEntity(worldEntities,
            entity::createSphere({0, -1000, 0}, 1000,
                                 material::createDiffuse({.5, 0.5, 0.5})));

  for (s32 a = -11; a < 11; a++) {
    for (s32 b = -11; b < 11; b++) {
      f32 chooseMat = math::rand01();
      math::vec3 center{a + 0.9f * math::rand01(), 0.2f,
                        b + 0.9f * math::rand01()};
      if ((center - math::vec3{4, 0.2, 0}).length() > 0.9) {
        if (chooseMat < 0.8) {
          addEntity(
              worldEntities,
              entity::createSphere(
                  center, 0.2,
                  material::createDiffuse({math::rand01() * math::rand01(),
                                           math::rand01() * math::rand01(),
                                           math::rand01() * math::rand01()})));

        } else if (chooseMat < 0.90) {
          addEntity(worldEntities,
                    entity::createSphere(
                        center, 0.2,
                        material::createMetal({0.5f * (1 + math::rand01()),
                                               0.5f * (1 + math::rand01()),
                                               0.5f * (1 + math::rand01())},
                                              1 - (0.5 * math::rand01()))));

        } else {
          addEntity(worldEntities,
                    entity::createSphere(center, 0.2,
                                         material::createDielectric(1.5)));
        }
      }
    }
  }

  addEntity(worldEntities, entity::createSphere(
                               {0, 1, 0}, 1, material::createDielectric(1.5)));
  addEntity(worldEntities,
            entity::createSphere({-4, 1, 0}, 1,
                                 material::createDiffuse({0.4, 0.2, 0.1})));
  addEntity(worldEntities,
            entity::createSphere({4, 1, 0}, 1,
                                 material::createMetal({0.7, 0.6, 0.5}, 1)));

  // vec3 up{0, 1, 0};
  // vec3 origin{13, 2, 3};
  // vec3 lookAt{0, 0, -1};
  // f32 aperture = 0.0;
  // f32 focusDistance = 10;
  // mainCamera = camera::createCamera(origin, lookAt, up, screenWidth,
  //                                   screenHeight, 20, aperture,
  //                                   focusDistance);
}
