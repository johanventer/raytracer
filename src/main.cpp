// NOTE(johan): This is the only place that includes any standard libraries,
// and so keeps them all in one spot so we can see what we are using.
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <pthread.h>

// NOTE(johan): 3rd party libraries
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <OpenGL/gl3ext.h>

#define USE_BVH 1

// TODO(johan): Better error handling
inline void fatal(const char* msg) {
  std::cerr << msg << "\n";
  exit(-1);
}

#include "types.h"
#include "math.h"
#include "camera.h"
#include "material.h"
#include "entity.h"
#include "entity_list.h"
#include "bvh.h"

struct Hit {
  f32 t;
  vec3 p;
  vec3 normal;
  material::Material* material;
};

// NOTE(johan): This is a "unity" build, there's only one translation unit and
// the linker has very little work to do.
#include "camera.cpp"
#include "material.cpp"
#include "entity.cpp"
#include "entity_list.cpp"
#include "bvh.cpp"

const u32 width = 1920 / 4;
const u32 height = 1080 / 4;
const u32 samples = 100;
const u32 maxDepth = 50;
u32 frameBuffer[width * height * 4];
vec3 sampledColor[width * height];
camera::Camera* mainCamera;
EntityList worldEntities;

#include "demo.cpp"

#if !USE_BVH
vec3 cast(const EntityList& entities, const camera::Ray& ray, u32 depth = 0) {
  Hit hit;

  // Epsilon for ignoring hits around t = 0
  f32 tMin = 0.001f;

  if (findHit(entities, ray, tMin, FLT_MAX, hit)) {
    camera::Ray scattered;
    vec3 attenuation;

    if (depth < maxDepth &&
        material::scatter(hit.material, ray, hit, attenuation, scattered)) {
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
#else
vec3 cast(const bvh::BoundingVolume* bvh,
          const camera::Ray& ray,
          u32 depth = 0) {
  Hit hit;

  // Epsilon for ignoring hits around t = 0
  f32 tMin = 0.001f;

  if (findHit(bvh, ray, tMin, FLT_MAX, hit)) {
    camera::Ray scattered;
    vec3 attenuation;

    if (depth < maxDepth &&
        material::scatter(hit.material, ray, hit, attenuation, scattered)) {
      return attenuation * cast(bvh, scattered, depth + 1);
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
#endif

inline ivec3 getSampledColor(u32 x, u32 y, u32 samples) {
  // Blending for antialiasing and gamma correction baked in here
  u32 pixelIndex = y * width + x;
  ivec3 result = {u32(255.99 * (sqrt(sampledColor[pixelIndex].r / samples))),
                  u32(255.99 * (sqrt(sampledColor[pixelIndex].g / samples))),
                  u32(255.99 * (sqrt(sampledColor[pixelIndex].b / samples)))};
  return result;
}

void* renderThread(void* data) {
#if USE_BVH
  auto bvh = new bvh::BoundingVolume(worldEntities);
  // bvh::printBvh(bvh);
#endif

  memset(sampledColor, 0, sizeof(sampledColor));

  u32 lastPercent = 0;
  for (s32 sampleIndex = 0; sampleIndex < samples; sampleIndex++) {
    u32 percent = f32(sampleIndex) / f32(samples) * 9.99f;
    if (percent != lastPercent) {
      lastPercent = percent;
      std::cerr << percent;
    }

    for (s32 y = height - 1; y >= 0; y--) {
      for (s32 x = 0; x < width; x++) {
        u32 pixelIndex = y * width + x;

        // Cast rays, collecting samples
        f32 u = f32(x + drand48()) / f32(width);
        f32 v = f32(y + drand48()) / f32(height);

        camera::Ray r = camera::ray(mainCamera, u, v);
#if USE_BVH
        vec3 color = cast(bvh, r);
#else
        vec3 color = cast(worldEntities, r);
#endif
        sampledColor[pixelIndex] += color;
      }
    }

    for (s32 y = height - 1; y >= 0; y--) {
      for (s32 x = 0; x < width; x++) {
        u32 pixelIndex = y * width + x;
        ivec3 color = getSampledColor(x, y, sampleIndex + 1);
        frameBuffer[pixelIndex] =
            (0xFF << 24) + (color.b << 16) + (color.g << 8) + color.r;
      }
    }
  }

  // Output PPM
  std::ofstream outfile("test.ppm", std::ios_base::out);
  outfile << "P3\n" << width << " " << height << "\n255\n";
  for (s32 y = height - 1; y >= 0; y--) {
    for (s32 x = 0; x < width; x++) {
      ivec3 color = getSampledColor(x, y, samples);
      outfile << color.r << " " << color.g << " " << color.b << "\n";
    }
  }
  outfile.close();

  //   std::cerr << std::endl;
  std::cerr << std::endl << "Render done!" << std::endl;
  return nullptr;
}

s32 main() {
  glfwSetErrorCallback(
      [](s32 error, const char* description) { fatal(description); });

  if (!glfwInit())
    fatal("GLFW initialization failed");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window;
  window = glfwCreateWindow(width, height, "Ray Tracer", NULL, NULL);
  if (!window) {
    glfwTerminate();
    fatal("Could not initialize glfw");
  }
  glfwMakeContextCurrent(window);

  glfwSwapInterval(1);
  // glViewport(0, 0, width, height);
  glDisable(GL_BLEND);

  std::cerr << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cerr << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;

  const GLchar* vertexShader =
      "#version 410 core\n"
      "const vec2 pos[4] = vec2[4](vec2(-1.0, 1.0), vec2(-1.0,-1.0), "
      "vec2(1.0, "
      "1.0), vec2(1.0,-1.0));"
      "out vec2 uv;"
      "void main() {"
      "  gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);"
      "  uv = 0.5 * pos[gl_VertexID] + vec2(0.5);"
      "}";
  const GLchar* fragmentShader =
      "#version 410 core\n"
      "uniform sampler2D sampler;"
      "in vec2 uv;"
      "out vec4 color;"
      "void main() {"
      "  color = texture(sampler, uv);"
      "}";
  GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  GLuint programId = glCreateProgram();
  GLint success;
  GLchar infoLog[1024];
  GLsizei length;
  glShaderSource(vertexShaderId, 1, &vertexShader, nullptr);
  glShaderSource(fragmentShaderId, 1, &fragmentShader, nullptr);
  glCompileShader(vertexShaderId);
  glCompileShader(fragmentShaderId);
  glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShaderId, 1024, &length, infoLog);
    std::cerr << infoLog << std::endl;
    fatal("Vertex shader failed to compile");
  }
  glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShaderId, 1024, &length, infoLog);
    std::cerr << infoLog << std::endl;
    fatal("Fragment shader failed to compile");
  }
  glAttachShader(programId, vertexShaderId);
  glAttachShader(programId, fragmentShaderId);
  glLinkProgram(programId);
  glGetProgramiv(programId, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(programId, 1024, &length, infoLog);
    std::cerr << infoLog << std::endl;
    fatal("Shader program failed to link");
  }

  GLuint textureId;
  glGenTextures(1, &textureId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  GLint sampler = glGetUniformLocation(programId, "sampler");
  glUniform1i(sampler, 0);

  GLuint vaoId;
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);

  glUseProgram(programId);

  memset(frameBuffer, 0, sizeof(frameBuffer));

  spheresWorld();
  // testWorld();
  // diffuseDemo();
  // metalDemo();
  // glassDemo();

  pthread_t thread;
  if (pthread_create(&thread, nullptr, renderThread, nullptr)) {
    fatal("Could not start render thread");
  }

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // for (u32 y = 0; y < height; y++)
    //   for (u32 x = 0; x < width; x++) {
    //     const u32 r = drand48() * 256;
    //     const u32 g = drand48() * 256;
    //     const u32 b = drand48() * 256;
    //     frameBuffer[width * y + x] = (0xFF << 24) + (b << 16) + (g << 8)
    //     + r;
    //   }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, frameBuffer);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // pthread_join(thread, nullptr);
}
