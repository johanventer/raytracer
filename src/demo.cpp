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

  vec3 up(0, 1, 0);
  vec3 origin(0, 1, 3);
  vec3 lookAt(0, 0, -1);
  f32 aperture = 0.1;
  f32 focusDistance = 2.5;  //(origin - lookAt).length();
  mainCamera = camera::createCamera(origin, lookAt, up, width, height, 60,
                                    aperture, focusDistance);
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

  vec3 up(0, 1, 0);
  vec3 origin(0, 2, 6);
  vec3 lookAt(0, 1.2, -1);
  f32 aperture = 0.1;
  f32 focusDistance = (origin - lookAt).length();
  mainCamera = camera::createCamera(origin, lookAt, up, 30, width, height,
                                    aperture, focusDistance);
}

void metalDemo() {
  addEntity(worldEntities,
            entity::createSphere(vec3(0, -1000, 0), 1000,
                                 material::createDiffuse(vec3(0.1, 0.1, 0.1))));

  addEntity(worldEntities, entity::createSphere(
                               vec3(-2, 1, -1), 1,
                               material::createMetal(vec3(0.5, 0.5, 0.5), 1)));
  addEntity(worldEntities, entity::createSphere(
                               vec3(0, 1, -1), 1,
                               material::createDiffuse(vec3(0.2, 0.45, 0.85))));
  addEntity(worldEntities, entity::createSphere(vec3(2, 1, -1), 1,
                                                material::createMetal(
                                                    vec3(0.5, 0.5, 0.5), 0.3)));

  vec3 up(0, 1, 0);
  vec3 origin(0, 2, 6);
  vec3 lookAt(0, 1.2, -1);
  f32 aperture = 0.1;
  f32 focusDistance = (origin - lookAt).length();
  mainCamera = camera::createCamera(origin, lookAt, up, width, height, 30,
                                    aperture, focusDistance);
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

  vec3 up(0, 1, 0);
  vec3 origin(0, 2, 6);
  vec3 lookAt(0, 1.2, -1);
  f32 aperture = 0.1;
  f32 focusDistance = (origin - lookAt).length();
  mainCamera = camera::createCamera(origin, lookAt, up, width, height, 30,
                                    aperture, focusDistance);
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

  vec3 up(0, 1, 0);
  vec3 origin(13, 2, 3);
  vec3 lookAt(0, 0, 0);
  f32 aperture = 0.0;
  f32 focusDistance = 10;
  mainCamera = camera::createCamera(origin, lookAt, up, width, height, 20,
                                    aperture, focusDistance);
}
