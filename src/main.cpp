// NOTE(johan): This is the only place that includes any standard libraries,
// and so keeps them all in one spot so we can see what we are using.
#include <float.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <atomic>
#include <future>

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

struct Scatterable;

struct Hit {
  f32 t;
  math::vec3 p;
  math::vec3 normal;
  Scatterable* material;
};

struct Hitable {
  INTERFACE(Hitable);

  virtual bool hit(const math::Ray& ray,
                   const f32 tMin,
                   const f32 tMax,
                   Hit& hit) const = 0;
};

struct Scatterable {
  INTERFACE(Scatterable);

  virtual bool scatter(const math::Ray& ray,
                       const Hit& hit,
                       math::vec3& attenuation,
                       math::Ray& scattered) const = 0;
};

struct Boundable {
  INTERFACE(Boundable);

  virtual bool bounds(math::AABB& box) const = 0;
};

#include "camera.h"
#include "material.h"
#include "entity.h"
#include "bvh.h"

// NOTE(johan): This is a "unity" build, there's only one translation unit and
// the linker has very little work to do.
#include "camera.cpp"
#include "material.cpp"
#include "entity.cpp"
#include "bvh.cpp"

// TODO(johan): These globals will eventually be driven by something else
// and will go away.
const u32 screenWidth = 1920 / 2;
const u32 screenHeight = 1080 / 2;
const u32 maxDepth = 50;

const u32 numPixels = screenWidth * screenHeight;
volatile std::atomic_bool quitting(false);
volatile std::atomic_bool moving(false);
camera::Camera mainCamera;
math::vec4 sampledColor[numPixels];
entity::EntityList worldEntities;

#include "demo.cpp"

std::string readFile(std::string fileName) {
  std::ifstream file(fileName);
  if (!file.good()) {
    fatal("Could not open file");
  }
  std::string result;
  file.seekg(0, std::ios::end);
  result.reserve(file.tellg());
  file.seekg(0, std::ios::beg);
  result.assign(std::istreambuf_iterator<char>(file),
                std::istreambuf_iterator<char>());
  return result;
}

GLuint loadShader(std::string fileName, GLenum shaderType) {
  auto source = readFile(fileName);
  auto source_c = source.c_str();
  GLuint id = glCreateShader(shaderType);
  GLint success;
  GLchar infoLog[1024];
  GLsizei length;
  glShaderSource(id, 1, &source_c, nullptr);
  glCompileShader(id);
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(id, 1024, &length, infoLog);
    std::cerr << "Failed to compile " << fileName << ": " << infoLog
              << std::endl;
    exit(1);
  }
  return id;
}

GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
  GLint success;
  GLchar infoLog[1024];
  GLsizei length;
  GLuint id = glCreateProgram();
  glAttachShader(id, vertexShader);
  glAttachShader(id, fragmentShader);
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id, 1024, &length, infoLog);
    std::cerr << "Failed to link shader program: " << infoLog << std::endl;
    exit(-1);
  }
  return id;
}

math::vec3 background(const math::Ray& ray) {
  math::vec3 unit_direction = math::normalize(ray.direction);
  f32 t = 0.5f * (unit_direction.y + 1);
  return math::lerp({1, 1, 1}, {0.5, 0.7, 1}, t);
}

math::vec3 cast(const bvh::BoundingVolume* bvh,
                const math::Ray& ray,
                u32 depth = 0) {
  Hit hit;

  // Epsilon for ignoring hits around t = 0
  f32 tMin = 0.001f;

  if (bvh->hit(ray, tMin, FLT_MAX, hit)) {
    math::Ray scattered;
    math::vec3 attenuation;

    if (depth < maxDepth &&
        hit.material->scatter(ray, hit, attenuation, scattered)) {
      return attenuation * cast(bvh, scattered, depth + 1);
    } else {
      return {0, 0, 0};
    }
  }

  return background(ray);
}

void renderThreadMain() {
  auto bvh = new bvh::BoundingVolume(worldEntities);
  auto cores = std::thread::hardware_concurrency();
  std::vector<std::future<void>> futures;
  u32 threadPixels = screenWidth * screenHeight / cores;

  for (u32 coreIndex = 0; coreIndex < cores; coreIndex++) {
    futures.emplace_back(std::async([&, coreIndex]() {
      u32 sampleCount = 1;
      u32 index = coreIndex;
      u32 count = 0;
      while (!quitting) {
        u32 x = index % screenWidth;
        u32 y = index / screenWidth;
        f32 u = f32(x + math::rand01()) / f32(screenWidth);
        f32 v = f32(y + math::rand01()) / f32(screenHeight);
        math::Ray r = mainCamera.ray(u, v);
        math::vec3 color = cast(bvh, r);
        if (sampleCount == 1) {
          sampledColor[index] = {color.x, color.y, color.z, 1};
        } else {
          sampledColor[index] = {
              sampledColor[index].x + color.x, sampledColor[index].y + color.y,
              sampledColor[index].z + color.z, f32(sampleCount)};
        }
        index += cores;
        if (++count >= threadPixels) {
          sampleCount++;
          count = 0;
          index = coreIndex;
        }
        if (moving || mainCamera.distanceVel != 0.0) {
          sampleCount = 1;
        }
      }
    }));
  }

  for (auto& future : futures) {
    future.wait();
  }
}

inline math::ivec3 getScreenColor(s32 x, s32 y) {
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

void saveScreenshot() {
  std::ofstream outfile("test.ppm", std::ios_base::out);
  outfile << "P3\n" << screenWidth << " " << screenHeight << "\n255\n";
  for (s32 y = screenHeight - 1; y >= 0; y--) {
    for (s32 x = 0; x < screenWidth; x++) {
      math::ivec3 color = getScreenColor(x, y);
      outfile << color.r << " " << color.g << " " << color.b << "\n";
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
      moving = true;
      rightButtonDown = true;
      glfwGetCursorPos(window, &startx, &starty);
    } else if (action == GLFW_RELEASE) {
      rightButtonDown = false;
      moving = false;
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
      mainCamera.lookAt +=
          offsetx * mainCamera.distance * 0.001f * mainCamera.right +
          -offsety * mainCamera.distance * 0.001f * mainCamera.up;
    } else {
      mainCamera.yaw += offsetx * 0.4;
      mainCamera.pitch += offsety * 0.4;
    }
  }
}

void handleMouseScroll(GLFWwindow* window, f64 x, f64 y) {
  mainCamera.distanceVel = -y * 3;
}

std::unique_ptr<std::thread> renderThread;

void restartRender() {
  quitting = true;
  if (renderThread.get()) {
    renderThread->join();
  }
  quitting = false;
  renderThread.reset(new std::thread(renderThreadMain));
  memset(sampledColor, 0, sizeof(sampledColor));
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
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
    glfwSetWindowShouldClose(window, 1);
  }
}

GLenum glCheckError_(const char* file, int line) {
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    std::string error;
    switch (errorCode) {
      case GL_INVALID_ENUM:
        error = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error = "INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        error = "INVALID_OPERATION";
        break;
      case GL_OUT_OF_MEMORY:
        error = "OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error = "INVALID_FRAMEBUFFER_OPERATION";
        break;
    }
    std::cerr << "GL ERROR: " << error << " | " << file << " (" << line << ")"
              << std::endl;
  }
  return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

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

  glfwSetKeyCallback(window, handleKeys);
  glfwSetMouseButtonCallback(window, handleMouseButton);
  glfwSetCursorPosCallback(window, handleMouseMove);
  glfwSetScrollCallback(window, handleMouseScroll);

  s32 width, height;
  glfwGetFramebufferSize(window, &width, &height);

  glfwSwapInterval(1);
  glViewport(0, 0, width, height);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  std::cerr << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cerr << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;

  auto vertexShader = loadShader("src/vertex.glsl", GL_VERTEX_SHADER);
  auto fragmentShader = loadShader("src/fragment.glsl", GL_FRAGMENT_SHADER);
  auto program = createShaderProgram(vertexShader, fragmentShader);
  glUseProgram(program);

  GLuint texture;
  glGenTextures(1, &texture);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glUniform1i(glGetUniformLocation(program, "sampledColor"), 0);

  GLuint vaoId;
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);

  glCheckError();

  spheresWorld();
  // diffuseDemo();
  // metalDemo();
  // glassDemo();
  // simpleDemo();
  // testDemo();

  mainCamera = camera::Camera(screenWidth, screenHeight, 20, 30, 0, 1, 0, 0);

  restartRender();

  f64 dt = 0;
  f64 frameTarget = 1.0 / 60.0;
  f64 startOfFrame = glfwGetTime();
  f64 frameTime = 0;
  f64 timeAccum = 0;

  while (!glfwWindowShouldClose(window)) {
    dt = glfwGetTime() - startOfFrame;
    startOfFrame = glfwGetTime();

    mainCamera.update(dt);

    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0,
                 GL_RGBA, GL_FLOAT, sampledColor);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glfwSwapBuffers(window);
    glfwPollEvents();

    frameTime = glfwGetTime() - startOfFrame;
    if (frameTime < frameTarget) {
      f64 sleepTime = frameTarget - frameTime;
      frameTime += sleepTime;
      std::this_thread::sleep_for(
          std::chrono::milliseconds(u32(sleepTime * 1000)));
    }

    // processInput(window, frameTime);

    timeAccum += frameTime;
    if (timeAccum > 1) {
      timeAccum = 0;
      std::cerr << "ms/frame: " << (frameTime * 1000) << std::endl;
    }
  }

  std::cerr << "Shutting down..." << std::endl;
  quitting = true;
  renderThread->join();
}
