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
