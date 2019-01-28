// NOTE(johan): This is the only place that includes any standard libraries,
// and so keeps them all in one spot so we can see what we are using.
#include <float.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <atomic>
#include <future>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <iomanip>
#include <random>

// NOTE(johan): 3rd party libraries
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr const char* scenesFolder = "scenes";
constexpr const char* screenshotsFolder = "screenshots";
constexpr const char* imagesFolder = "images";

#include "types.h"

static inline void fatal(const char* msg) {
  std::cerr << msg << "\n";
  exit(-1);
}

#include "math.h"

namespace material {
struct Material;
}

struct Hit {
  f32 t;
  math::vec3 p;
  math::vec3 normal;
  material::Material* material;
  f32 u, v;
};

struct Hitable {
  INTERFACE(Hitable);

  virtual bool hit(const math::Ray& ray,
                   const f32 tMin,
                   const f32 tMax,
                   Hit& hit) const = 0;
};

struct Samplable {
  INTERFACE(Samplable);

  virtual math::vec3 sample(f32 u, f32 v, const math::vec3& p) const = 0;
};

struct Scatterable {
  INTERFACE(Scatterable);

  virtual bool scatter(const math::Ray& ray,
                       const Hit& hit,
                       math::vec3& attenuation,
                       math::Ray& scattered) const = 0;
};

struct Emissive {
  INTERFACE(Emissive);

  virtual math::vec3 emit(f32 u, f32 v, const math::vec3& p) const = 0;
};

struct Boundable {
  INTERFACE(Boundable);

  virtual bool bounds(math::AABB& box) const = 0;
};

struct ImGuiInspectable {
  INTERFACE(ImGuiInspectable);

  virtual bool renderInspector() = 0;
};

#include "camera.h"
#include "texture.h"
#include "material.h"
#include "entity.h"
#include "bvh.h"

// NOTE(johan): This is a "unity" build, there's only one translation unit and
// the linker has very little work to do.
#include "util.cpp"
#include "camera.cpp"
#include "texture.cpp"
#include "material.cpp"
#include "entity.cpp"
#include "bvh.cpp"
#include "serializer.cpp"

// TODO(johan): These globals will eventually be driven by something else
// and will go away.
const u32 screenWidth = 1920 / 2;
const u32 screenHeight = 1080 / 2;
const u32 maxDepth = 50;
const u32 numPixels = screenWidth * screenHeight;
std::atomic_bool quitting(false);
std::atomic_bool cameraMoving(false);
std::unique_ptr<camera::Camera> mainCamera;
math::vec4 sampledColor[numPixels];
entity::EntityList worldEntities;
std::unique_ptr<std::thread> renderThread;
auto cores = std::thread::hardware_concurrency();
f64 threadSampleTimes[8];
u32 threadSampleCounts[8];
f64 threadSampleProfileTimes[8][100];
f64 mouseStartx, mouseStarty;
bool rightButtonDown = false;
struct {
  bool showFrameTime = true;
  math::vec3 backgroundTop = {0, 0, 0};
  math::vec3 backgroundBottom = {0, 0, 0};
} guiState;
struct {
  bool isProfiling;
  const char* profileScene;
} options;
constexpr auto maxProfileSamples = 100;

#include "demo.cpp"

math::vec3 background(const math::Ray& ray) {
  math::vec3 unit_direction = math::normalize(ray.direction);
  f32 t = 0.5f * (unit_direction.y + 1);
  return math::lerp(guiState.backgroundBottom, guiState.backgroundTop, t);
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

    if (hit.material) {
      math::vec3 emitted = hit.material->emit(hit.u, hit.v, hit.p);
      if (depth < maxDepth &&
          hit.material->scatter(ray, hit, attenuation, scattered)) {
        return emitted + attenuation * cast(bvh, scattered, depth + 1);
      } else {
        return emitted;
      }
    } else {
      return {0, 0, 0};
    }
  }

  return background(ray);
}

void renderThreadMain() {
  auto bvh = new bvh::BoundingVolume(worldEntities);
  std::vector<std::future<void>> futures;
  u32 threadPixels = screenWidth * screenHeight / cores;

  for (u32 coreIndex = 0; coreIndex < cores; coreIndex++) {
    futures.emplace_back(std::async([&, coreIndex]() {
      u32 sampleCount = 1;
      threadSampleCounts[coreIndex] = 0;
      u32 index = coreIndex;
      u32 count = 0;
      auto startTime = glfwGetTime();

      while (!quitting) {
        u32 x = index % screenWidth;
        u32 y = index / screenWidth;
        f32 u = f32(x + math::rand01()) / f32(screenWidth);
        f32 v = f32(y + math::rand01()) / f32(screenHeight);
        math::Ray r = mainCamera->ray(u, v);
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
          // Take some timing for profiling
          auto sampleTime = glfwGetTime() - startTime;
          if (sampleCount - 1 < maxProfileSamples)
            threadSampleProfileTimes[coreIndex][sampleCount - 1] = sampleTime;
          threadSampleTimes[coreIndex] = sampleTime;
          threadSampleCounts[coreIndex]++;

          // Reset for next sample
          sampleCount++;
          count = 0;
          index = coreIndex;
          startTime = glfwGetTime();
        }

        if (cameraMoving || mainCamera->distanceVel != 0.0) {
          sampleCount = 1;
          threadSampleCounts[coreIndex] = 0;
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
      u32(255.99f * math::clamp(sqrt(sampledColor[pixelIndex].r /
                                     f32(sampledColor[pixelIndex].a)),
                                0, 1)),
      u32(255.99f * math::clamp(sqrt(sampledColor[pixelIndex].g /
                                     f32(sampledColor[pixelIndex].a)),
                                0, 1)),
      u32(255.99f * math::clamp(sqrt(sampledColor[pixelIndex].b /
                                     f32(sampledColor[pixelIndex].a)),
                                0, 1))};
  return result;
}

void saveScreenshot() {
  createScreenshotsDirectory();

  std::ostringstream ss;
  auto now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  ss << screenshotsFolder << "/"
     << std::put_time(std::localtime(&now), "%F.%H.%M.%S") << ".ppm";

  std::ofstream outfile(ss.str(), std::ios_base::out);
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

void handleMouseButton(GLFWwindow* window, s32 button, s32 action, s32 mods) {
  if (!ImGui::IsMouseHoveringAnyWindow()) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (action == GLFW_PRESS) {
        cameraMoving = true;
        rightButtonDown = true;
        glfwGetCursorPos(window, &mouseStartx, &mouseStarty);
      } else if (action == GLFW_RELEASE) {
        rightButtonDown = false;
        cameraMoving = false;
      }
    }
  }
}

void handleMouseMove(GLFWwindow* window, f64 x, f64 y) {
  if (rightButtonDown && !ImGui::IsMouseHoveringAnyWindow()) {
    f32 offsetx = mouseStartx - x;
    f32 offsety = mouseStarty - y;
    mouseStartx = x;
    mouseStarty = y;

    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      mainCamera->lookAt +=
          offsetx * mainCamera->distance * 0.001f * mainCamera->right +
          -offsety * mainCamera->distance * 0.001f * mainCamera->up;
    } else {
      mainCamera->yaw += offsetx * 0.4;
      mainCamera->pitch += offsety * 0.4;
    }
  }
}

void handleMouseScroll(GLFWwindow* window, f64 x, f64 y) {
  if (!ImGui::IsMouseHoveringAnyWindow()) {
    mainCamera->distanceVel = -y * 0.2 * mainCamera->distance;
  }
}

void stopRender() {
  quitting = true;
  if (renderThread.get() && renderThread.get()->joinable()) {
    renderThread->join();
  }
}

void restartRender() {
  stopRender();
  quitting = false;
  renderThread.reset(new std::thread(renderThreadMain));
  // memset(sampledColor, 0, sizeof(sampledColor));
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

void guiOverlay(f64 dt) {
  ImGui::SetNextWindowPos({0, 0});
  ImGui::SetNextWindowSize({screenWidth, screenHeight});
  ImGui::Begin("Overlay", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                   ImGuiWindowFlags_NoBackground);
  {
    if (guiState.showFrameTime) {
      ImGui::TextColored({0.5, 0.5, 0.5, 1.0}, "%fms", dt * 1000);
    }
  }
  ImGui::End();
}

void emptyScene() {
  worldEntities.add(
      new entity::Sphere({0, -1000, 0}, 999,
                         new material::Diffuse(new texture::Checker(
                             2, {0.1, 0.1, 0.1}, {0.9, 0.9, 0.9}))));
  mainCamera.reset(
      new camera::Camera(screenWidth, screenHeight, 20, 30, 0, 1, -15, 0));

  mainCamera->update(0);
}

void cornellScene() {
  material::Material* red =
      new material::Diffuse(new texture::Solid({0.65f, 0.05f, 0.05f}));
  material::Material* white =
      new material::Diffuse(new texture::Solid({0.73f, 0.73f, 0.73f}));
  material::Material* green =
      new material::Diffuse(new texture::Solid({0.12f, 0.45f, 0.15f}));
  material::Material* light =
      new material::DiffuseLight(new texture::Solid({1, 1, 1}), 10);

  // Green wall left
  worldEntities.add(new entity::FlipNormals(
      new entity::YZRectangle({555, 277.5f, 277.5f}, 555, 555, green)));

  // Red wall right
  worldEntities.add(
      new entity::YZRectangle({0, 277.5f, 277.5f}, 555, 555, red));

  // Back wall
  worldEntities.add(new entity::FlipNormals(
      new entity::XYRectangle({277.5f, 277.5f, 555}, 555, 555, white)));

  // Floor
  worldEntities.add(
      new entity::XZRectangle({277.5f, 0, 277.5f}, 555, 555, white));

  // Ceiling
  worldEntities.add(new entity::FlipNormals(
      new entity::XZRectangle({277.5f, 555, 277.5f}, 555, 555, white)));

  // Light
  worldEntities.add(
      new entity::XZRectangle({278, 554, 279.5f}, 250, 250, light));

  // Small Box
  worldEntities.add(
      new entity::Box({212.5, 82.5, 147.5}, 165, 165, 165, white));

  // Large Box
  worldEntities.add(new entity::Box({347.5, 165, 377.4}, 165, 330, 165, white));

  mainCamera.reset(
      new camera::Camera(screenWidth, screenHeight, 800, 40, 0, 10, 0, 0));
  mainCamera->lookAt = {278, 278, 0};

  mainCamera->update(0);
}

void newScene() {
  stopRender();
  worldEntities.clear();
  emptyScene();
  restartRender();
}

void loadScene(std::string scene) {
  stopRender();
  worldEntities.clear();
  camera::Camera* newCamera = nullptr;

  std::ostringstream ss;
  ss << scenesFolder << "/" << scene << ".txt";

  deserializeScene(ss.str(), worldEntities, newCamera, screenWidth,
                   screenHeight);
  mainCamera.reset(newCamera);
  mainCamera->update(0);
  restartRender();
}

void saveScene(std::string scene) {
  scene = trim(scene);
  if (scene.length()) {
    std::ostringstream ss;
    ss << scenesFolder << "/" << scene << ".txt";
    createSceneDirectory();
    serializeScene(ss.str(), worldEntities, mainCamera.get());
  }
}

void guiTabs(f64 dt) {
  static bool showLoadScenePopup = false;
  static bool showSaveScenePopup = false;

  ImGui::Begin("Ray Tracer", nullptr, ImGuiWindowFlags_MenuBar);
  {
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New scene")) {
          newScene();
        }
        if (ImGui::MenuItem("Load scene")) {
          showLoadScenePopup = true;
        }
        ImGui::SameLine();
        ImGui::ShowHelpMarker(
            "Scenes are loaded from a scenes/ folder in the current working "
            "directory and have a .txt extension.");
        if (ImGui::MenuItem("Save scene")) {
          showSaveScenePopup = true;
        }
        ImGui::SameLine();
        ImGui::ShowHelpMarker(
            "Scenes are saved to a scenes/ folder in the current working "
            "directory. They all have .txt extensions, which you don't "
            "need to specify.");

        ImGui::Separator();

        if (ImGui::MenuItem("Take screenshot")) {
          saveScreenshot();
        };
        ImGui::SameLine();
        ImGui::ShowHelpMarker(
            "Screenshots are saved to a screenshots/ folder in the current "
            "working directory.");

        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

    if (showLoadScenePopup) {
      ImGui::OpenPopup("Load scene popup");
    }

    if (ImGui::BeginPopupModal("Load scene popup", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
      auto sceneFiles = findScenes();
      for (auto& sceneFile : sceneFiles) {
        if (ImGui::MenuItem(sceneFile.c_str())) {
          loadScene(sceneFile);
          showLoadScenePopup = false;
          ImGui::CloseCurrentPopup();
        }
      }
      ImGui::Indent(ImGui::GetWindowContentRegionWidth() - 50);
      if (ImGui::Button("Cancel")) {
        showLoadScenePopup = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    if (showSaveScenePopup) {
      ImGui::OpenPopup("Save scene popup");
    }

    if (ImGui::BeginPopupModal("Save scene popup", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
      char fileName[1024] = "new-scene";
      ImGui::Text("File name:");
      if (!ImGui::IsAnyItemActive())
        ImGui::SetKeyboardFocusHere();
      if (ImGui::InputText("##filename", fileName, LENGTH(fileName),
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        saveScene(fileName);
        showSaveScenePopup = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::Indent(ImGui::GetWindowContentRegionWidth() - 50);
      if (ImGui::Button("Cancel")) {
        showSaveScenePopup = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    ImGui::BeginTabBar("Main Tabs");
    {
      if (ImGui::BeginTabItem("Scene")) {
        if (ImGui::CollapsingHeader("Background",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Text("Background top:");
          if (ImGui::ColorEdit3("##background top", guiState.backgroundTop.e)) {
            restartRender();
          }
          ImGui::Text("Background bottom:");
          if (ImGui::ColorEdit3("##background bottom",
                                guiState.backgroundBottom.e)) {
            restartRender();
          }
        }

        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Camera")) {
        if (mainCamera->renderInspector()) {
          restartRender();
        }
        ImGui::EndTabItem();
      }

      s32 entityIndexToRemove = -1;

      if (ImGui::BeginTabItem("Entities")) {
        ImGui::Button("Add");
        if (ImGui::BeginPopupContextItem(nullptr, 0)) {
          for (u32 entityTypeIndex = u32(entity::EntityType::start) + 1;
               entityTypeIndex < u32(entity::EntityType::end);
               entityTypeIndex++) {
            auto type = entity::EntityType(entityTypeIndex);
            const char* name = entity::toString(type);
            if (ImGui::MenuItem(name)) {
              worldEntities.add(entity::createEntity(type));
              restartRender();
            }
          }
          ImGui::EndPopup();
        }

        for (u32 entityIndex = 0; entityIndex < worldEntities.size();
             entityIndex++) {
          entity::Entity* entity = worldEntities.at(entityIndex);

          bool opened = ImGui::TreeNode((void*)entity, "%s",
                                        entity::toString(entity).c_str());
          ImGui::PushID((void*)entity);
          if (ImGui::BeginPopupContextItem("Entity Popup")) {
            if (ImGui::MenuItem("Remove")) {
              entityIndexToRemove = entityIndex;
            };
            ImGui::EndPopup();
          }
          ImGui::PopID();
          if (opened) {
            if (entity->renderInspector()) {
              restartRender();
            }

            bool opened = ImGui::TreeNode(
                "Material", "Material: %s",
                entity->material ? material::toString(entity->material->type())
                                 : "none");
            if (entity->material) {
              ImGui::PushID("Material");
              if (ImGui::BeginPopupContextItem("Material Popup")) {
                if (ImGui::MenuItem("Remove")) {
                  stopRender();
                  if (entity->material->texture) {
                    delete entity->material->texture;
                  }
                  delete entity->material;
                  entity->material = nullptr;
                  restartRender();
                }
                ImGui::EndPopup();
              }
              ImGui::PopID();
            }
            if (opened) {
              if (entity->material) {
                if (entity->material->renderInspector()) {
                  restartRender();
                }

                bool opened = ImGui::TreeNode(
                    "Texture", "Texture: %s",
                    entity->material->texture
                        ? texture::toString(entity->material->texture->type())
                        : "none");
                if (entity->material->texture) {
                  ImGui::PushID("Texture");
                  if (ImGui::BeginPopupContextItem("Texture Popup")) {
                    if (ImGui::MenuItem("Remove")) {
                      stopRender();
                      delete entity->material->texture;
                      entity->material->texture = nullptr;
                      restartRender();
                    }
                    ImGui::EndPopup();
                  }
                  ImGui::PopID();
                }
                if (opened) {
                  if (entity->material->texture) {
                    if (entity->material->texture->renderInspector()) {
                      restartRender();
                    }
                  } else {
                    ImGui::Button("Add");
                    if (ImGui::BeginPopupContextItem(nullptr, 0)) {
                      for (u32 textureTypeIndex =
                               u32(texture::TextureType::start) + 1;
                           textureTypeIndex < u32(texture::TextureType::end);
                           textureTypeIndex++) {
                        auto type = texture::TextureType(textureTypeIndex);
                        const char* name = texture::toString(type);
                        if (ImGui::MenuItem(name)) {
                          entity->material->texture =
                              texture::createTexture(type);
                          restartRender();
                        }
                      }
                      ImGui::EndPopup();
                    }
                  }
                  ImGui::TreePop();
                }
              } else {
                ImGui::Button("Add");
                if (ImGui::BeginPopupContextItem(nullptr, 0)) {
                  for (u32 materialTypeIndex =
                           u32(material::MaterialType::start) + 1;
                       materialTypeIndex < u32(material::MaterialType::end);
                       materialTypeIndex++) {
                    auto type = material::MaterialType(materialTypeIndex);
                    const char* name = material::toString(type);
                    if (ImGui::MenuItem(name)) {
                      entity->material = material::createMaterial(type);
                      restartRender();
                    }
                  }
                  ImGui::EndPopup();
                }
              }
              ImGui::TreePop();
            }
            ImGui::TreePop();
          }
        }
        ImGui::EndTabItem();
      }

      if (entityIndexToRemove > -1) {
        stopRender();
        worldEntities.remove(entityIndexToRemove);
        restartRender();
      }

      if (ImGui::BeginTabItem("Profile")) {
        if (ImGui::CollapsingHeader("Overlay",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Checkbox("Frame ms:", &guiState.showFrameTime);
          ImGui::SameLine();
          ImGui::Text("%fms", dt * 1000);
        }
        if (ImGui::CollapsingHeader("Thread Sample Count",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          auto cores = std::thread::hardware_concurrency();
          for (u32 coreIndex = 0; coreIndex < cores; coreIndex++) {
            ImGui::TreeNode((void*)(intptr_t)coreIndex, "Thread %d: %d",
                            coreIndex, threadSampleCounts[coreIndex]);
          }
        }
        if (ImGui::CollapsingHeader("Thread Sample Time",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          auto cores = std::thread::hardware_concurrency();
          for (u32 coreIndex = 0; coreIndex < cores; coreIndex++) {
            ImGui::TreeNode((void*)(intptr_t)coreIndex, "Thread %d: %fms",
                            coreIndex, threadSampleTimes[coreIndex] * 1000);
          }
        }
        ImGui::EndTabItem();
      }
    }
  }
  ImGui::EndTabBar();
  ImGui::End();
}

void runProfile(const char* scene) {
  std::cerr << "Profile running..." << std::endl;

  bool isDone = false;

  loadScene(scene);

  while (!isDone) {
    for (auto coreIndex = 0; coreIndex < cores; coreIndex++) {
      isDone = true;
      if (threadSampleCounts[coreIndex] < maxProfileSamples) {
        isDone = false;
      }
    }

    if (isDone) {
      stopRender();
      std::cerr << std::endl;

      f64 threadAverageSampleTime[cores];
      f64 overallAverageSampleTime = 0;
      for (auto coreIndex = 0; coreIndex < cores; coreIndex++) {
        threadAverageSampleTime[coreIndex] = 0;

        for (auto sampleIndex = 0; sampleIndex < maxProfileSamples;
             sampleIndex++) {
          threadAverageSampleTime[coreIndex] +=
              threadSampleProfileTimes[coreIndex][sampleIndex];
        }

        threadAverageSampleTime[coreIndex] /= maxProfileSamples;
        overallAverageSampleTime += threadAverageSampleTime[coreIndex];

        std::cerr << "Thread " << coreIndex << " avg sample time: "
                  << threadAverageSampleTime[coreIndex] * 1000 << "ms"
                  << std::endl;
      }

      std::cerr << "Overall avg. sample time: "
                << overallAverageSampleTime * 1000 << "ms" << std::endl;
      break;
    }

    for (auto coreIndex = 0; coreIndex < cores; coreIndex++) {
      f32 percent = threadSampleCounts[0] / f32(maxProfileSamples) * 100.f;
      std::cerr << percent << "%\r";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void parseCommandLine(int argc, const char** argv) {
  u32 argumentIndex = 1;
  if (argc > 1) {
    if (strcmp("--profile", argv[argumentIndex++]) == 0) {
      options.isProfiling = true;

      if (argumentIndex < argc) {
        options.profileScene = argv[argumentIndex++];
      } else {
        fatal("If specifying --profile, you must specify a scene name");
      }
    }
  }
}

int main(int argc, const char** argv) {
  glfwSetErrorCallback(
      [](s32 error, const char* description) { fatal(description); });

  if (!glfwInit())
    fatal("GLFW initialization failed");

  parseCommandLine(argc, argv);

  if (options.isProfiling) {
    runProfile(options.profileScene);
    return 0;
  }

  const char* glsl_version = "#version 140";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow* window;
  window =
      glfwCreateWindow(screenWidth, screenHeight, "Ray Tracer", NULL, NULL);
  if (!window) {
    glfwTerminate();
    fatal("Could not initialize glfw");
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (gladLoadGL() == 0) {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return 1;
  }

  glfwSetKeyCallback(window, handleKeys);
  glfwSetMouseButtonCallback(window, handleMouseButton);
  glfwSetCursorPosCallback(window, handleMouseMove);
  glfwSetScrollCallback(window, handleMouseScroll);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.MouseDrawCursor = true;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

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

  // spheresWorld();
  // diffuseDemo();
  // metalDemo();
  // glassDemo();
  // simpleDemo();
  // testDemo();
  cornellScene();
  restartRender();

  f64 dt = 0;
  f64 frameTarget = 1.0 / 60.0;
  f64 startOfFrame = glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
    startOfFrame = glfwGetTime();

    glfwPollEvents();

    mainCamera->update(dt);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    guiOverlay(dt);
    guiTabs(dt);

    ImGui::Render();
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0,
                 GL_RGBA, GL_FLOAT, sampledColor);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);

    dt = glfwGetTime() - startOfFrame;
    if (dt < frameTarget) {
      f64 sleepTime = frameTarget - dt;
      dt += sleepTime;
      std::this_thread::sleep_for(
          std::chrono::milliseconds(u32(sleepTime * 1000)));
    }
  }

  std::cerr << "Shutting down..." << std::endl;
  quitting = true;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  renderThread->join();
}
