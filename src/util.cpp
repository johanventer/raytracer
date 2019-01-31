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

void createSceneDirectory() {
  if (mkdir(scenesFolder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
    if (errno != EEXIST) {
      fatal("Failed to create the scenes folder.");
    }
  }
}

void createScreenshotsDirectory() {
  if (mkdir(screenshotsFolder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
    if (errno != EEXIST) {
      fatal("Failed to create the screenshots folder.");
    }
  }
}

inline std::string trim(const std::string& s) {
  auto wsfront = std::find_if_not(s.begin(), s.end(),
                                  [](int c) { return std::isspace(c); });
  auto wsback = std::find_if_not(s.rbegin(), s.rend(),
                                 [](int c) { return std::isspace(c); })
                    .base();
  return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}

bool endsWith(std::string const& fullString, std::string const& ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(),
                                    ending.length(), ending));
  } else {
    return false;
  }
}

std::vector<std::string> findFiles(std::string dirName,
                                   std::string extension = "") {
  std::vector<std::string> files;
  dirent* dirEntry;
  auto dir = opendir(dirName.c_str());
  if (dir) {
    while ((dirEntry = readdir(dir)) != NULL) {
      auto file = std::string(dirEntry->d_name);
      if (file != "." && file != ".." && endsWith(file, extension)) {
        if (extension != "" && endsWith(file, extension)) {
          file = file.substr(0, file.length() - 4);
        }
        files.push_back(file);
      }
    }
    closedir(dir);
  }
  return files;
}

std::vector<std::string> findScenes() {
  return findFiles(scenesFolder, ".txt");
}

std::vector<std::string> findImages() {
  return findFiles(imagesFolder);
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

namespace ImGui {
static void ShowHelpMarker(const char* desc) {
  TextDisabled("(?)");
  if (IsItemHovered()) {
    BeginTooltip();
    PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    TextUnformatted(desc);
    PopTextWrapPos();
    EndTooltip();
  }
}

static bool Vec3ColorEdit(const char* label, math::vec3& v) {
  static f32 e[3];
  e[0] = v.x();
  e[1] = v.y();
  e[2] = v.z();
  if (ColorEdit3(label, e)) {
    v = math::vec3(e);
    return true;
  }

  return false;
}

static bool Vec3DragFloat(const char* label,
                          math::vec3& v,
                          f32 step = 1,
                          f32 min = 0,
                          f32 max = 0) {
  static f32 e[3];
  e[0] = v.x();
  e[1] = v.y();
  e[2] = v.z();
  if (DragFloat3(label, e, step, min, max)) {
    v = math::vec3(e);
    return true;
  }

  return false;
}
}  // namespace ImGui