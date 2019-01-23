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
#include <libkern/OSAtomic.h>

// NOTE(johan): 3rd party libraries
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

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
  math::vec3 p;
  math::vec3 normal;
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
const u32 maxSamples = 100;
const u32 maxDepth = 50;
const u32 renderThreads = 4;
u32 frameBuffer[screenWidth * screenHeight];
math::vec4 sampledColor[screenWidth * screenHeight];
camera::Camera* mainCamera;
EntityList worldEntities;

#include "demo.cpp"

math::vec3 background(const camera::Ray& ray) {
  math::vec3 unit_direction = math::normalize(ray.direction);
  f32 t = 0.5f * (unit_direction.y + 1);
  return math::lerp({1, 1, 1}, {0.5, 0.7, 1}, t);
}

math::vec3 cast(const bvh::BoundingVolume* bvh,
                const camera::Ray& ray,
                u32 depth = 0) {
  Hit hit;

  // Epsilon for ignoring hits around t = 0
  f32 tMin = 0.001f;

  if (findHit(bvh, ray, tMin, FLT_MAX, hit)) {
    camera::Ray scattered;
    math::vec3 attenuation;

    if (depth < maxDepth &&
        material::scatter(hit.material, ray, hit, attenuation, scattered)) {
      return attenuation * cast(bvh, scattered, depth + 1);
    } else {
      return {0, 0, 0};
    }
  }

  return background(ray);
}

inline math::ivec3 getScreenColor(u32 x, u32 y) {
  // NOTE(johan): Blending for antialiasing and gamma correction baked in here
  u32 pixelIndex = y * screenWidth + x;
  math::ivec3 result = {
      u32(255.99f *
          sqrt(sampledColor[pixelIndex].r / f32(sampledColor[pixelIndex].a))),
      u32(255.99f *
          sqrt(sampledColor[pixelIndex].g / f32(sampledColor[pixelIndex].a))),
      u32(255.99f *
          sqrt(sampledColor[pixelIndex].b / f32(sampledColor[pixelIndex].a)))};
  return result;
}

struct RenderThreadData {
  u32 id;
  u32 start;
  u32 width;
  volatile s32 sampleIndex;
  bvh::BoundingVolume* bvh;
};

void* renderThread(void* _data) {
  const auto data = (RenderThreadData*)_data;

  while (true) {
    while (data->sampleIndex < maxSamples) {
      // NOTE(johan): Snap a copy of the sample index
      auto sampleIndex = data->sampleIndex;

      for (s32 y = screenHeight - 1; y > 0; y--) {
        for (s32 x = data->start; x < data->start + data->width; x++) {
          u32 pixelIndex = y * screenWidth + x;

          f32 u = f32(x + math::rand01()) / f32(screenWidth);
          f32 v = f32(y + math::rand01()) / f32(screenHeight);
          camera::Ray r = camera::ray(mainCamera, u, v);
          math::vec3 color = cast(data->bvh, r);

          if (sampleIndex == 0) {
            sampledColor[pixelIndex] = {color.x, color.y, color.z, 1};
          } else {
            sampledColor[pixelIndex] = {sampledColor[pixelIndex].x + color.x,
                                        sampledColor[pixelIndex].y + color.y,
                                        sampledColor[pixelIndex].z + color.z,
                                        f32(sampleIndex + 1)};
          }

          math::ivec3 screenColor = getScreenColor(x, y);
          frameBuffer[pixelIndex] = (0xFF << 24) + (screenColor.b << 16) +
                                    (screenColor.g << 8) + screenColor.r;
        }
      }

      // NOTE(johan): If the thread's sample index has been changed outside of
      // this loop, break out
      if (!OSAtomicCompareAndSwap32(sampleIndex, sampleIndex + 1,
                                    &data->sampleIndex)) {
        break;
      }
    }

    // NOTE(johan): Sleep this thread until there's something to render again
    while (data->sampleIndex != 0) {
      usleep(1000);
    }
  }

  return nullptr;
}

pthread_t threads[renderThreads];
RenderThreadData threadData[renderThreads];

void restartRender() {
  // TODO(johan): Still feel this is ham fisted, even with the atomic compare
  // and swap in the threads themselves. But if will do for now.
  for (u32 threadIndex = 0; threadIndex < renderThreads; threadIndex++) {
    threadData[threadIndex].sampleIndex = 0;
  }
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

f64 startx, starty;
bool rightButtonDown = false;

void handleMouseButton(GLFWwindow* window, s32 button, s32 action, s32 mods) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      rightButtonDown = true;
      glfwGetCursorPos(window, &startx, &starty);
    } else if (action == GLFW_RELEASE) {
      rightButtonDown = false;
    }
  }
}

void handleMouseMove(GLFWwindow* window, f64 x, f64 y) {
  if (rightButtonDown) {
    f32 offsetx = startx - x;
    f32 offsety = starty - y;
    startx = x;
    starty = y;

    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
      mainCamera->lookAt +=
          offsetx * mainCamera->distance * 0.001f * mainCamera->right +
          -offsety * mainCamera->distance * 0.001f * mainCamera->up;
    } else {
      mainCamera->yaw += offsetx * 0.4;
      mainCamera->pitch += offsety * 0.4;
    }

    camera::updateCamera(*mainCamera);
    restartRender();
  }
}

void handleMouseScroll(GLFWwindow* window, f64 x, f64 y) {
  f32 sensitivity = 1;
  f32 offsety = y * sensitivity;

  mainCamera->distance -= offsety;
  camera::updateCamera(*mainCamera);
  restartRender();
}

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

  s32 left, top, right, bottom;
  glfwGetWindowFrameSize(window, &left, &top, &right, &bottom);

  glfwSetKeyCallback(window, handleKeys);
  glfwSetMouseButtonCallback(window, handleMouseButton);
  glfwSetCursorPosCallback(window, handleMouseMove);
  glfwSetScrollCallback(window, handleMouseScroll);

  glfwSwapInterval(1);
  glViewport(0, 0, bottom - top, right - left);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH);

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
  diffuseDemo();
  // metalDemo();
  // glassDemo();
  // simpleDemo();
  // testDemo();

  mainCamera =
      camera::createCamera(screenWidth, screenHeight, 20, 30, 0, 1, 0, 0);

  auto bvh = new bvh::BoundingVolume(worldEntities);

  memset(frameBuffer, 0, sizeof(frameBuffer));
  memset(sampledColor, 0, sizeof(sampledColor));

  restartRender();

  u32 threadWidth = screenWidth / renderThreads;
  u32 remainder = screenWidth - renderThreads * threadWidth;

  for (u32 threadIndex = 0; threadIndex < renderThreads; threadIndex++) {
    threadData[threadIndex] = {.id = threadIndex,
                               .bvh = bvh,
                               .start = threadIndex * threadWidth,
                               .width = threadIndex == renderThreads - 1
                                            ? threadWidth + remainder
                                            : threadWidth};

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
    glfwSwapBuffers(window);
    glfwPollEvents();

    endOfFrame = glfwGetTime();
    frameTime = endOfFrame - startOfFrame;

    if (frameTime < frameTarget) {
      f64 sleepTime = frameTarget - frameTime;
      frameTime += sleepTime;
      usleep(sleepTime * 1000000);
    }

    // processInput(window, frameTime);

    timeAccum += frameTime;
    if (timeAccum > 1) {
      timeAccum = 0;
      std::cerr << "ms/frame: " << (frameTime * 1000) << std::endl;
    }
  }
}
