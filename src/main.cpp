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
#include <unistd.h>

// NOTE(johan): 3rd party libraries
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#ifndef USE_BVH
#define USE_BVH
#endif

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

// TODO(johan): These globals will eventually be driven by something else
// and will go away.
const u32 screenWidth = 1920 / 2;
const u32 screenHeight = 1080 / 2;
const u32 samples = 10;
const u32 maxDepth = 50;
const u32 renderThreads = 2;
u32 frameBuffer[screenWidth * screenHeight];
vec3 sampledColor[screenWidth * screenHeight];
camera::Camera* mainCamera;
EntityList worldEntities;

#include "demo.cpp"

vec3 background(const camera::Ray& ray) {
  vec3 unit_direction = normalize(ray.direction);
  f32 t = 0.5f * (unit_direction.y + 1);
  return lerp({1, 1, 1}, {0.5, 0.7, 1}, t);
}

#ifndef USE_BVH
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
      return {0, 0, 0};
    }
  }

  return background(ray);
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
      return {0, 0, 0};
    }
  }

  return background(ray);
}
#endif

inline ivec3 getSampledColor(u32 x, u32 y, u32 sampleCount) {
  // Blending for antialiasing and gamma correction baked in here
  u32 pixelIndex = y * screenWidth + x;
  ivec3 result = {
      u32(255.99f * sqrt(sampledColor[pixelIndex].r / sampleCount)),
      u32(255.99f * sqrt(sampledColor[pixelIndex].g / sampleCount)),
      u32(255.99f * sqrt(sampledColor[pixelIndex].b / sampleCount))};
  return result;
}

struct RenderThreadData {
  u32 id;
  u32 start;
  u32 width;
#ifdef USE_BVH
  bvh::BoundingVolume* bvh;
#endif

  // Outputs from the thread
  u32 sampleIndex;
};

// u32 lastPercent = 0;
//   u32 percent = f32(sampleIndex) / f32(samples) * 9.99f;
//   if (percent != lastPercent) {
//     lastPercent = percent;
//     std::cerr << "Thread " << data->id << ": " << percent << "0%"
//               << std::endl;
//   }
// std::cerr << std::endl << "Thread " << data->id << " done!" << std::endl;

void* renderThread(void* _data) {
  const auto data = (RenderThreadData*)_data;

  while (true) {
    for (; data->sampleIndex < samples; data->sampleIndex++) {
      for (s32 y = screenHeight - 1; y > 0; y--) {
        for (s32 x = data->start; x < data->start + data->width; x++) {
          u32 pixelIndex = y * screenWidth + x;

          // Cast rays, collecting samples
          f32 u = f32(x + rand01()) / f32(screenWidth);
          f32 v = f32(y + rand01()) / f32(screenHeight);
          camera::Ray r = camera::ray(mainCamera, u, v);
#ifdef USE_BVH
          vec3 color = cast(data->bvh, r);
#else
          vec3 color = cast(worldEntities, r);
#endif
          if (data->sampleIndex == 0) {
            sampledColor[pixelIndex] = color;
          } else {
            sampledColor[pixelIndex] += color;
          }
        }
      }

      for (s32 y = screenHeight - 1; y > 0; y--) {
        for (s32 x = data->start; x < data->start + data->width; x++) {
          u32 pixelIndex = y * screenWidth + x;
          ivec3 color = getSampledColor(x, y, data->sampleIndex + 1);
          frameBuffer[pixelIndex] =
              (0xFF << 24) + (color.b << 16) + (color.g << 8) + color.r;
        }
      }
    }

    std::cerr << "Render thread " << data->id << " waiting...\n";
    while (data->sampleIndex != 0) {
      usleep(100000);
    }
  }

  return nullptr;
}

void saveScreenshot() {
  std::ofstream outfile("test.ppm", std::ios_base::out);
  outfile << "P3\n" << screenWidth << " " << screenHeight << "\n255\n";
  for (s32 y = screenHeight - 1; y >= 0; y--) {
    for (s32 x = 0; x < screenWidth; x++) {
      u32 color = frameBuffer[x + y * screenWidth];
      outfile << (color & 0x000000FF) << " " << ((color & 0x0000FF00) >> 8)
              << " " << ((color & 0x00FF0000) >> 16) << "\n";
    }
  }
  outfile.close();
  std::cerr << "Screenshot taken." << std::endl;
}

f64 lastMouseX = screenWidth / 2, lastMouseY = screenHeight / 2;
bool firstMouse = true;
// bool rightButtonDown = false;

pthread_t threads[renderThreads];
RenderThreadData threadData[renderThreads];

void restartRender() {
  // TODO(johan): MUCH better signalling of threads required, this is stupid and
  // error prone
  for (u32 threadIndex = 0; threadIndex < renderThreads; threadIndex++) {
    threadData[threadIndex].sampleIndex = 0;
  }
}

// void handleMouseButton(GLFWwindow* window, s32 button, s32 action, s32 mods)
// {
//   if (button == GLFW_MOUSE_BUTTON_RIGHT) {
//     if (action == GLFW_PRESS) {
//       rightButtonDown = true;
//       glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
//     } else if (action == GLFW_RELEASE) {
//       rightButtonDown = false;
//     }
//   }
// }

void handleMouseMove(GLFWwindow* window, f64 x, f64 y) {
  // if (rightButtonDown) {
  if (firstMouse) {
    lastMouseX = x;
    lastMouseY = y;
    firstMouse = false;
  }

  f32 offsetX = x - lastMouseX;
  f32 offsetY = lastMouseY - y;
  lastMouseX = x;
  lastMouseY = y;
  f32 sensitivity = 0.4;
  offsetX *= sensitivity;
  offsetY *= sensitivity;

  mainCamera->yaw += offsetX;
  mainCamera->pitch += offsetY;

  camera::updateCamera(*mainCamera);
  restartRender();
  // }
}

void handleMouseScroll(GLFWwindow* window, f64 x, f64 y) {}

void handleKeys(GLFWwindow* window,
                s32 key,
                s32 scanCode,
                s32 action,
                s32 mods) {
  if (key == GLFW_KEY_S && action == GLFW_RELEASE &&
      (mods & GLFW_MOD_CONTROL)) {
    saveScreenshot();
  }
  if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
    restartRender();
  }
}

void processInput(GLFWwindow* window, f32 dt) {
  f32 cameraSpeed = 4 * dt;
  bool shouldUpdateCamera = false;

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    mainCamera->origin += cameraSpeed * mainCamera->front;
    shouldUpdateCamera = true;
  }

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    mainCamera->origin -= cameraSpeed * mainCamera->front;
    shouldUpdateCamera = true;
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    mainCamera->origin -= cameraSpeed * mainCamera->right;
    shouldUpdateCamera = true;
  }

  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    mainCamera->origin += cameraSpeed * mainCamera->right;
    shouldUpdateCamera = true;
  }

  if (shouldUpdateCamera) {
    camera::updateCamera(*mainCamera);
    restartRender();
  }
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
  window =
      glfwCreateWindow(screenWidth, screenHeight, "Ray Tracer", NULL, NULL);
  if (!window) {
    glfwTerminate();
    fatal("Could not initialize glfw");
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetKeyCallback(window, handleKeys);
  // glfwSetMouseButtonCallback(window, handleMouseButton);
  glfwSetCursorPosCallback(window, handleMouseMove);

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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  GLint sampler = glGetUniformLocation(programId, "sampler");
  glUniform1i(sampler, 0);

  GLuint vaoId;
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);

  glUseProgram(programId);

  // spheresWorld();
  // testWorld();
  diffuseDemo();
  // metalDemo();
  // glassDemo();

#ifdef USE_BVH
  auto bvh = new bvh::BoundingVolume(worldEntities);
#endif

  memset(frameBuffer, 0, sizeof(frameBuffer));
  memset(sampledColor, 0, sizeof(sampledColor));

  u32 threadWidth = screenWidth / renderThreads;
  u32 remainder = screenWidth - renderThreads * threadWidth;

  for (u32 threadIndex = 0; threadIndex < renderThreads; threadIndex++) {
    threadData[threadIndex] = {.id = threadIndex,
                               .start = threadIndex * threadWidth,
                               .width = threadIndex == renderThreads - 1
                                            ? threadWidth + remainder
                                            : threadWidth};

#ifdef USE_BVH
    threadData[threadIndex].bvh = bvh;
#endif

    std::cerr << "Render thread " << threadIndex << ": ["
              << threadData[threadIndex].start << "," << 0 << "] -> ["
              << (threadData[threadIndex].start + threadData[threadIndex].width)
              << "," << screenHeight << "]\n";

    if (pthread_create(&threads[threadIndex], nullptr, renderThread,
                       &threadData[threadIndex])) {
      fatal("Could not start render thread");
    }
  }

  f64 frameTarget = 1.0 / 60.0;
  f64 startOfFrame;
  f64 endOfFrame;
  f64 frameTime;
  f64 timeAccum = 0;

  while (!glfwWindowShouldClose(window)) {
    startOfFrame = glfwGetTime();

    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glfwPollEvents();
    glfwSwapBuffers(window);

    endOfFrame = glfwGetTime();
    frameTime = endOfFrame - startOfFrame;

    if (frameTime < frameTarget) {
      f64 sleepTime = frameTarget - frameTime;
      frameTime += sleepTime;
      usleep(sleepTime * 1000000);
    }

    processInput(window, frameTime);

    timeAccum += frameTime;
    if (timeAccum > 1) {
      timeAccum = 0;
      std::cerr << "ms/frame: " << (frameTime * 1000) << std::endl;
    }
  }
}
