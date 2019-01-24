#version 410 core

const vec2 pos[4] = vec2[4](
  vec2(-1.0, 1.0), 
  vec2(-1.0,-1.0), 
  vec2(1.0, 1.0), 
  vec2(1.0,-1.0)
);

out vec2 uv;

void main() {
  gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
  uv = 0.5 * pos[gl_VertexID] + vec2(0.5);
}
